#include <svgpp/svgpp.hpp>
#include <rapidxml_ns/rapidxml_ns_utils.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>
#include <algorithm>
#include <boost/geometry/geometry.hpp>
#include <sstream>

#include "Parser.hpp"
#include "Shape.hpp"
#include "Solver.hpp"

using namespace std;
using namespace rapidxml_ns;
using namespace svgpp;
#define BEZIER_STEP 0.01 //Precision of bezier interpolation

/**
 * Fills shapes with the different interpolated
 * shapes found in the SVG file.
 */
vector<Shape> Parser::Parse(string path,
                            vector<string>& ids,
                            Point& docDim) { //Returns a copy, can be improved
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
    cerr << "Path move to " << x << "," << y << endl;
}

/**
 * Draw a straight line from the current point to a new point, which
 * is the new current point.
 */
void Parser::path_line_to(double x, double y, svgpp::tag::coordinate::absolute) {
    //This should only store the point as a new shape point.
    _points.emplace_back(x, y);
    cerr << "Path line to " << x << "," << y << endl;
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
    Point p0 = _points.back();

    for (double t = BEZIER_STEP ; t <= 1 ; t += BEZIER_STEP) {
        double nx = p0.x() * (1 - t) * (1 - t) * (1 - t) + 3 * x1 * t * (1 - t) * (1 - t) +
                    3 * x2 * t * t * (1 - t) + x * t * t * t;
        double ny = p0.y() * (1 - t) * (1 - t) * (1 - t) + 3 * y1 * t * (1 - t) * (1 - t) +
                    3 * y2 * t * t * (1 - t) + y * t * t * t;
        _points.emplace_back(nx, ny);
    }

    cerr << "Path cubic bezier (" << x1 << "," << y1 << ") ; (" <<
         x2 << "," << y2 << ") ; (" << x << "," << y << ")" << endl;
}

/**
 * Ends the current subpath and causes an automatic straight line to be
 * drawn from the current point to the initial point of the current
 * subpath.
 */
void Parser::path_close_subpath() {
    //Nothing to do as the initial point should already be added
    cerr << "Close subpath" << endl;
}

/**
 * Actions taken at the end of a path.
 */
void Parser::path_exit() {
    //This should probably send all the accumulated points to a new Shape
    //and add it to the shape vector.
    cerr << "Path exit (" << _groupStack << ")\n";

    //Reverse the points if the polygon has the wrong orientation
    if (bg::area(Ring(_points.begin(), _points.end())) < 0) {
        reverse(_points.begin(), _points.end());
    }



    _rings.emplace_back(_points.begin(), _points.end());
}

/**
 * Beginning of a new group.
 */
void Parser::on_enter_element(svgpp::tag::element::g) {
    cerr << "Element enter (group)" << endl;
    //Pushing identity
    _matStack.push(_matStack.top());

    for (int i = _rings.size() - 1 ; i >= 0 ; i--) {
        //Iterate through the parsed rings (in reverse order to match the stack)
        cerr << "Flushing rings..." << endl;

        for (auto && p : _rings[i]) {
            p = _matStack.top()(p);
        }

        vector<Ring> tmp {_rings[i]};

        if (_ids.empty() or vectorContains(_ids, _idStack.top())) {
            //Add the ring to _shapes only if the ID on top of the stack (its own ID)
            //is in the _ids vector (or if there is no ID specified by the user)
            _shapes.emplace_back(tmp, _idStack.top());
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
    //Pushing identity
    _matStack.push(_matStack.top());
    cerr << "Element enter (" << _groupStack << ")\n";

    if (_groupStack >= 0) {
        _groupStack++;
    }

    _points.clear();
}

/**
 * End of a group or any other element.
 */
void Parser::on_exit_element() {
    cerr << "Element exit (" << _groupStack << ")\n";

    //If we are closing a group, ignore all the ids of its components
    if (_groupStack == 0) {
        for (unsigned i = 0 ; i < _rings.size() ; i++) {
            _idStack.pop();
        }
    }

    if (_groupStack <= 0 && !_rings.empty()) {

        //Add the ring to _shapes only if the ID on top of the stack (its own ID)
        //is in the _ids vector (or if there is no ID specified by the user)
        if (_ids.empty() or vectorContains(_ids, _idStack.top())) {

            for (auto && points : _rings)
                for (auto && p : points) {
                    p = _matStack.top()(p);
                }

            _shapes.emplace_back(_rings, _idStack.top());
        }

        _idStack.pop();
        _rings.clear();
    }

    _groupStack--;
    Matrix tmp = _matStack.top();
    _matStack.pop();

    if (tmp != _matStack.top()) {
        cerr << "Popped a real transformation matrix : " << tmp << endl;
        _matStack.pop();
    }
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

    cerr << "Current ID : " << _idStack.top() << endl;
}

/**
 * Parsing the height of the dock.
 */
void Parser::set(svgpp::tag::attribute::height, double height) {
    cerr << "Parsed " << height << " as doc height" << endl;
    _docDim.set<1>(height);
}

/**
 * Parsing the width of the dock.
 */
void Parser::set(svgpp::tag::attribute::width, double width) {
    cerr << "Parsed " << width << " as doc width" << endl;
    _docDim.set<0>(width);
}

/**
 * Parse the current transform matrix.
 */
void Parser::transform_matrix(const boost::array<double, 6>& matrix) {
    _matStack.append(Matrix(matrix));
    cerr << "New transformation state : " << _matStack.top() << " (stack size : ";
    cerr << _matStack.size() << ")"	<< endl;
}
