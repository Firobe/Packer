#include "Parser.h"
#include "Shape.h"

#include <rapidxml_ns/rapidxml_ns.hpp>
#include <rapidxml_ns/rapidxml_ns_utils.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>

using namespace std;
using namespace rapidxml_ns;
using namespace svgpp;

/**
 * Fills shapes with the different interpolated
 * shapes found in the SVG file.
 */
void Parser::fillShapes(vector<Shape>& shapes) {
    //Opening SVG file
    file<> svgFile(_path.c_str());
    xml_document<> doc;
    doc.parse<0>(svgFile.data());

    //Parse the XML
    xml_node<>* root_node = doc.first_node();
    Context context;
    /*document_traversal <processed_elements<tag::element::path>,
                       processed_attributes<traits::shapes_attributes_by_element>
                       >::load_document(root_node, context);*/
    }
