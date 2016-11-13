#include "Parser.h"
#include "Shape.h"

#include <rapidxml_ns/rapidxml_ns_utils.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>

using namespace std;
using namespace rapidxml_ns;
using namespace svgpp;

/**
 * Fills shapes with the different interpolated
 * shapes found in the SVG file.
 */
vector<Shape> Parser::Parse(string path) { //Returns a copy, can be improved
    //Opening SVG file
    file<> svgFile(path.c_str());
    xml_document<> doc;
    doc.parse<0>(svgFile.data());

    //Parse the XML
    XMLElement rootNode = doc.first_node();
    Parser context;
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
    ///TODO
    ///This should only store the point as an initial subpath point
    cout << "Path move to " << x << "," << y << endl;
}

/**
 * Draw a straight line from the current point to a new point, which
 * is the new current point.
 */
void Parser::path_line_to(double x, double y, svgpp::tag::coordinate::absolute) {
    ///TODO
    //This should only store the point as a new shape point.
    cout << "Path line to " << x << "," << y << endl;
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
    ///TODO
    //This should interpolate the curve with a given precision
    //and add the interpolated points to the shape
    cout << "Path cubic bezier (" << x1 << "," << y1 << ") ; (" <<
         x2 << "," << y2 << ") ; (" << x << "," << y << ")" << endl;
}

/**
 * Ends the current subpath and causes an automatic straight line to be
 * drawn from the current point to the initial point of the current
 * subpath.
 */
void Parser::path_close_subpath() {
    ///TODO
    //This should only call path_line_to with the correct point
    cout << "Close subpath" << endl;
}

void Parser::path_exit() {
    ///TODO
    //This should probably send all the accumulated points to a new Shape
    //and add it to the shape vector.
    cout << "Path exit" << endl;
}

void Parser::on_enter_element(svgpp::tag::element::any) {
    ///TODO
    //This should clear any accumulated points.
    cout << "Element enter" << endl;
}

void Parser::on_exit_element() {
    ///Nothing to do if there is only one path by element
    cout << "Element exit" << endl;
}
