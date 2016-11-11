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
    document_traversal <error_policy<IgnoreError>,
                       processed_elements<ProcessedElements>,
                       processed_attributes<traits::shapes_attributes_by_element>
                       >::load_document(rootNode, context);
    return context.getShapes();
}

/**
 * The following methods should fill _shapes with correct interpolated points,
 * accordingly to the SVG specification
 */

void Parser::path_move_to(double x, double y, svgpp::tag::coordinate::absolute) {
    ///TODO
    cout << "Path move" << endl;
}

void Parser::path_line_to(double x, double y, svgpp::tag::coordinate::absolute) {
    ///TODO
    cout << "Path line" << endl;
}

void Parser::path_cubic_bezier_to(
    double x1, double y1,
    double x2, double y2,
    double x, double y,
    svgpp::tag::coordinate::absolute) {
    ///TODO
    cout << "Path cubic bezier" << endl;
}

void Parser::path_quadratic_bezier_to(
    double x1, double y1,
    double x, double y,
    svgpp::tag::coordinate::absolute) {
    ///TODO
    cout << "Path quadratic bezier" << endl;
}

void Parser::path_elliptical_arc_to(
    double rx, double ry, double x_axis_rotation,
    bool large_arc_flag, bool sweep_flag,
    double x, double y,
    svgpp::tag::coordinate::absolute) {
    ///TODO
    cout << "Path elliptical arc" << endl;
}

void Parser::path_close_subpath() {
    ///TODO
    cout << "Close subpath" << endl;
}

void Parser::path_exit() {
    ///TODO
    cout << "Path exit" << endl;
}

void Parser::on_enter_element(svgpp::tag::element::any) {
    ///TODO
    cout << "Element enter" << endl;
}

void Parser::on_exit_element() {
    ///TODO
    cout << "Element exit" << endl;
}
