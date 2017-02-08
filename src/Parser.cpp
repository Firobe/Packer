#include <algorithm>
#include <sstream>

#include <svgpp/svgpp.hpp>
#include <rapidxml_ns/rapidxml_ns_utils.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>

#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/strategies/cartesian/area_surveyor.hpp>
#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/algorithms/length.hpp>

#include "Parser.hpp"
#include "Shape.hpp"
#include "Log.hpp"
#include "Rotos.hpp"

using namespace std;
using namespace rapidxml_ns;
using namespace svgpp;

std::vector<std::string> Parser::_identifiers;

/**
 * Fills shapes with the different interpolated
 * shapes found in the SVG file.
 */
vector<Shape> Parser::Parse(string path,
                            vector<string>& ids,
                            Point& docDim) { // TODO : Returns a copy, can be improved
    LOG(info) << "Parsing SVG file..." << endl;
    //Opening SVG file
    file<> svgFile(path.c_str());
    xml_document<> doc;
    doc.parse<0>(svgFile.data());
    //Parse the XML
    XMLElement rootNode = doc.first_node();
    Parser context(ids, docDim);
    document_traversal <error_policy<IgnoreError>, //Enables IgnoreError
                       path_policy<policy::path::minimal>, /* Enables approximation of all types of curved paths
															 to cubic bezier paths */
                       processed_elements<ProcessedElements>,
                       processed_attributes<ProcessedAttributes> //Transform matrix not handled
                       >::load_document(rootNode, context);
    LOG(info) << "File successfully parsed." << endl;
    return context.getShapes();
}

/**
 * The following methods should fill _shapes with correct interpolated points,
 * accordingly to the SVG specification and recreate a SVG string that will
 * later be outputed. All coordinates are absolute.
 */

/**
 * Establish a new current point.
 */
void Parser::path_move_to(double x, double y, svgpp::tag::coordinate::absolute) {
    ///This should only store the point as an initial subpath point
    _points.emplace_back(x, y);
    LOG(trace) << "Path move to " << x << "," << y << endl;
}

/**
 * Draw a straight line from the current point to a new point, which
 * is the new current point.
 */
void Parser::path_line_to(double x, double y, svgpp::tag::coordinate::absolute) {
    //This should only store the point as a new shape point.
    _points.emplace_back(x, y);
    LOG(trace) << "Path line to " << x << "," << y << endl;
}

/**
 * Computes the middle of the [p1 p2] segment
 */
Point middlePoint(Point& p1, Point& p2) {
    Point r(p1);
    bg::add_point(r, p2);
    bg::divide_value(r, 2.);
    return r;
}

/**
 * Returns a sufficiently accurate interpolation of a Bezier curve
 * defined by p1 p4 its anchor points and p2 p3 its control points,
 * using a recursive algorithm (Casteljau)
 */
vector<Point> subdivision(Point& p1, Point& p2, Point& p3, Point& p4) {
    //Computing points defining subdivised curves
    //Names follow http://www.antigrain.com/research/adaptive_bezier/bezier06.gif
    Point p12(middlePoint(p1, p2));
    Point p23(middlePoint(p2, p3));
    Point p34(middlePoint(p3, p4));
    Point p123(middlePoint(p12, p23));
    Point p234(middlePoint(p23, p34));
    Point p1234(middlePoint(p123, p234));
    //Estimating the flatness of our current curve
    double interpolatedX, interpolatedY;
    double norm = rotos::norm(p1.x(), p1.y(), p2.x(), p2.y(), p3.x(), p3.y(), p4.x(), p4.y(),
                              interpolatedX, interpolatedY);

    if (norm < BEZIER_TOLERANCE) { //If our curve is flat enough
        return {Point(interpolatedX, interpolatedY)};
        //We pick the extremum returned by rotos
    }
    else {
        //If not flat enough, continue subdivision and concatenate vectors
        return subdivision(p1, p12, p123, p1234) + subdivision(p1234, p234, p34, p4);
    }
}

/**
 * Draws a cubic Bézier curbe from the current point to (x, y)
 * using (x1, y1) as the control point at the beginning of the
 * curbe and (x2, y2) as the control point at the end of the curve.
 * (x, y) becomes the current point.
 */
void Parser::path_cubic_bezier_to(
    double x1, double y1,
    double x2, double y2,
    double x, double y,
    svgpp::tag::coordinate::absolute) {
    Point p1(x1, y1), p2(x2, y2), p3(x, y);
    vector<Point> interpolated = subdivision(_points.back(), p1, p2, p3);
    _points.reserve(_points.size() + interpolated.size());
    _points.insert(_points.end(), interpolated.begin(), interpolated.end());
    LOG(trace) << "Path cubic bezier (" << x1 << "," << y1 << ") ; (" <<
               x2 << "," << y2 << ") ; (" << x << "," << y << ")" << endl;
}

/**
 * Ends the current subpath and causes an automatic straight line to be
 * drawn from the current point to the initial point of the current
 * subpath.
 */
void Parser::path_close_subpath() {
    //Nothing to do as the initial point should already be added
    LOG(trace) << "Close subpath" << endl;
}

/**
 * Actions taken at the end of a path.
 */
void Parser::path_exit() {
    //This should probably send all the accumulated points to a new Shape
    //and add it to the shape vector.
    LOG(trace) << "Path exit (" << _groupStack << ")\n";
    _toApply++;
    _rings.emplace_back(_points.begin(), _points.end());
    //Reverse the points if the ring has the wrong orientation and
    //close the ring if it isn't.
    bg::correct(_rings.back());
}

/**
 * Beginning of a new group.
 */
void Parser::on_enter_element(svgpp::tag::element::g) {
    LOG(debug) << "Element enter (group)" << endl;

    //i need to be signed because of the loop test
    for (int i = _rings.size() - 1 ; i >= 0 ; --i) {
        //Iterate through the parsed rings (in reverse order to match the stack)
        LOG(debug) << "Flushing rings..." << endl;
        vector<Ring> tmp {_rings[i]};

        if (_ids.empty() or vectorContains(_ids, _idStack.top())) {
            //Add the ring to _shapes only if the ID on top of the stack (its own ID)
            //is in the _ids vector (or if there is no ID specified by the user)
            _identifiers.push_back(_idStack.top());
            _shapes.emplace_back(tmp, _identifiers.size() - 1);
        }

        _idStack.pop();
    }

    _rings.clear();
    _groupStack = 0;
}

/**
 * Beginning of a new shape (or unknown element)
 */
void Parser::on_enter_element(svgpp::tag::element::any) {
    LOG(debug) << "Element enter (" << _groupStack << ")\n";
    _currentMatrix = Matrix(1, 0, 0, 1, 0, 0);

    if (_groupStack >= 0) {
        _groupStack++;
    }

    _points.clear();
}

/**
 * End of a group or any other element.
 */
void Parser::on_exit_element() {
    LOG(debug) << "Element exit (" << _groupStack << ")\n";

    //If we are closing a group, ignore all the ids of its components
    if (_groupStack == 0) {
        for (unsigned i = 0 ; i < _rings.size() ; ++i) {
            _idStack.pop();
        }
    }

    //Apply current matrix to recently generated points
    for (int i = (int)_rings.size() - 1 ; (unsigned)i >= _rings.size() - _toApply &&
            i >= 0; --i)
        for (auto && p : _rings[i]) {
            p = _currentMatrix(p);
        }

    _toApply = 0;

    if (_groupStack <= 0 && !_rings.empty()) {
        //Add the ring to _shapes only if the ID on top of the stack (its own ID)
        //is in the _ids vector (or if there is no ID specified by the user)
        if (_ids.empty() or vectorContains(_ids, _idStack.top())) {
            _identifiers.push_back(_idStack.top());
            _shapes.emplace_back(_rings, _identifiers.size() - 1);
        }

        _idStack.pop();
        _rings.clear();
    }

    _groupStack--;
}

/**
 * Parsing a new ID attribute.
 */
void Parser::set(svgpp::tag::attribute::id,
                 const boost::iterator_range<const char*> pId) {
    stringstream ss;
    ss << pId;
    //ss.str() now contains the new ID

    /**
     * If we are parsing the ID of a new group (_groupStack == 0) and that the ID of our current
     * group is not in the authorized IDs, we propagate that authorization and thus
     * we replace the old ID by our new ID in the list.
     */
    if (_groupStack == 0 && !_idStack.empty() && vectorContains(_ids, _idStack.top()) &&
            !vectorContains(_ids, ss.str())) {
        _ids.erase(remove(_ids.begin(), _ids.end(), _idStack.top()), _ids.end());
        _ids.push_back(ss.str());
    }

    //We add our new ID on the top of the stack
    _idStack.push(ss.str());

    //Ignoring layers
    if (_idStack.top().find("layer") != string::npos) {
        _groupStack = -1;
    }

    LOG(debug) << "Current ID : " << _idStack.top() << endl;
}

/**
 * Parsing the height of the dock.
 */
void Parser::set(svgpp::tag::attribute::height, double height) {
    LOG(debug) << "Parsed " << height << " as doc height" << endl;
    _docDim.set<1>(height);
}

/**
 * Parsing the width of the dock.
 */
void Parser::set(svgpp::tag::attribute::width, double width) {
    LOG(debug) << "Parsed " << width << " as doc width" << endl;
    _docDim.set<0>(width);
}

/**
 * Parse the current transform matrix.
 */
void Parser::transform_matrix(const boost::array<double, 6>& matrix) {
    _currentMatrix = Matrix(matrix);
    LOG(debug) << "New transformation state : " << _currentMatrix << " (stack size : ";
}
