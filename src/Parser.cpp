#include <svgpp/svgpp.hpp>
#include <rapidxml_ns/rapidxml_ns_utils.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>
#include <algorithm>
#include <boost/geometry/geometry.hpp>

#include "Parser.hpp"
#include "Shape.hpp"

using namespace std;
using namespace rapidxml_ns;
using namespace svgpp;
#define BEZIER_STEP 0.01

/**
 * Fills shapes with the different interpolated
 * shapes found in the SVG file.
 */
vector<Shape> Parser::Parse(string path,
                            vector<string> ids) { //Returns a copy, can be improved
    //Opening SVG file
    file<> svgFile(path.c_str());
    xml_document<> doc;
    doc.parse<0>(svgFile.data());

    //Parse the XML
    XMLElement rootNode = doc.first_node();
    Parser context(ids);
    document_traversal <error_policy<IgnoreError>, //Enables IgnoreError
                       path_policy<policy::path::minimal>, /* Enables approximation of all types of curved paths
															 to cubic bezier paths */
                       processed_elements<ProcessedElements>,
                       processed_attributes<traits::shapes_attributes_by_element> //Transform matrix not handled
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

    for (double t = 0 ; t <= 1 ; t += BEZIER_STEP) {
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

    if (_groupStack < 0) {
        _shapes.emplace_back(_rings);
        _rings.clear();
    }
}

/**
 * Beginning of a new group.
 */
void Parser::on_enter_element(svgpp::tag::element::g) {
    cerr << "Element enter (group)" << endl;

    for (auto r : _rings) {
        vector<Ring> tmp {r};
        _shapes.emplace_back(tmp);
    }

    _rings.clear();
    _groupStack = 0;
}

/**
 * Beginning of a new shape (or unknown element)
 */
void Parser::on_enter_element(svgpp::tag::element::any) {
    //This should clear any accumulated points.
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

    if (_groupStack == 0) {
        _shapes.emplace_back(_rings);
        _rings.clear();
    }

    _groupStack--;
}
