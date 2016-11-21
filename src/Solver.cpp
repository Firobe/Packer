#include <boost/geometry/io/svg/svg_mapper.hpp>
#include <rapidxml_ns/rapidxml_ns_print.hpp>
#include <rapidxml_ns/rapidxml_ns_utils.hpp>
#include <sstream>
#include <string>

#include "Solver.hpp"

using namespace std;
using namespace rapidxml_ns;
using XMLElement = rapidxml_ns::xml_node<>* ;  //Defines the XMLElement type

/**
 * Writes to ss the content of
 * a node : the beginning the tag, the name
 * and each of its attributes
 */
void printNode(stringstream& ss, XMLElement node) {
    ss << "<" << node->name() << endl;

    for (xml_attribute<>* attr = node->first_attribute();
            attr; attr = attr->next_attribute()) {
        ss << attr->name() << "=\"";
        ss << attr->value() << "\"" << endl;
    }

}

/**
 * Main output function. Recursive.
 * Display the node (and adds a transformation matrix if the node
 * is a shape which was packed by our algorithm).
 * Then calls itself on the children of the nodes.
 */
void Solver::recurOutput(stringstream& ss, xml_document<>& doc, vector<string>& ids,
                         XMLElement node, double offset, bool addto) {
    //Get the ID attribute of the node
    xml_attribute<>* id = node->first_attribute("id");
    char* cs = nullptr;

    //Check if that ID exists and is in our packed list
    if (id != nullptr && vectorContains<string>(ids, id->value())) {
        //Then creates the matrix and add it as an attribute
        cerr << "Displaying ID " << id->value() << endl;
        string s;
        int i;

        //Find to which shape (i) the ID belongs to
        for (i = 0 ; _shapes[i].getID() != id->value() ; i++);

        //Get the matrix and write its SVG string equivalent
        array<double, 6> m = _shapes[i].getTransMatrix();
	if (addto){
	  m[5] += offset;
	  printNode(ss, node);
	}
	
	s = "matrix(";

        for (int e = 0 ; e < 6 ; e++) {
            s += to_string(m[e]) + (e == 5 ? ")" : ", ");
        }

        //Allocate space and add it as an attribute
        cs = new char[s.size() + 1];
        strcpy(cs, s.c_str());
        cs[s.size()] = '\0';
        xml_attribute<>* mat = doc.allocate_attribute("transform", cs);
        node->prepend_attribute(mat);
    }
    else if (id != nullptr) {
        cerr << "Ignored ID " << id->value() << endl;
    }

    printNode(ss, node);

    if (cs != nullptr) {
        delete[] cs;
    }

    //Get the first child of the node
    XMLElement next = node->first_node();

    if (next != nullptr) {
        ss << ">" << endl;

        //Check if the tag is of the type <tag>raw text</tag>
        if (node->value_size() == 0)

            //If it isn't, call each of the children
            for (; next != nullptr ; next = next->next_sibling()) {
	      recurOutput(ss, doc, ids, next, offset, addto);
            }
        else {
            //If it is, just display the raw text
            ss << node->value() << endl;
        }

        //Add the closing tag
        ss << "</" << node->name() << ">" << endl;
    }
    else {
        //If there is no child, this is a <tag /> type tag.
        ss << "/>" << endl;
    }
}

/**
 * Returns a string corresponding to the SVG file to ouput.
 * (Theoretically with the packed shapes)
 */
string Solver::outputSVG(string path, bool addto, vector<string> ids) {
    //Opening SVG file
    file<> svgFile(path.c_str());
    xml_document<> doc;
    doc.parse<0>(svgFile.data());
  
    //Parse the XML
    XMLElement rootNode = doc.first_node();
    double windowHeight = stod(rootNode->first_attribute("height")->value());
    cerr << "Windows Height: " << windowHeight << endl;
    stringstream ss;
    recurOutput(ss, doc, ids, rootNode, windowHeight, addto);
    cerr << "SVG successfully generated" << endl;
    return ss.str();
}

/**
 * Output function using the svg output methods of BOOST.
 * Should be used for debug only.
 * Outputs what the solver actually sees.
 */
string Solver::debugOutputSVG() {
    stringstream ret;
    bg::svg_mapper<Point> mapper(ret, 800, 800);

    for (Shape& s : _shapes) {
        mapper.add(s.getMultiP());
    }

    for (Shape& s : _shapes) {
        mapper.map(s.getMultiP(), "fill:rgb(" + to_string(rand() % 256) + "," +
                   to_string(rand() % 256) + "," + to_string(rand() % 256) + ")");
    }

    cerr << "SVG successfully generated" << endl;
    return ret.str() + "</svg>";
}
