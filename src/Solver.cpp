#include <boost/geometry/io/svg/svg_mapper.hpp>
#include <rapidxml_ns/rapidxml_ns_print.hpp>
#include <rapidxml_ns/rapidxml_ns_utils.hpp>
#include <sstream>
#include <string>

#include "Solver.hpp"

using namespace std;
using namespace rapidxml_ns;
using XMLElement = rapidxml_ns::xml_node<>* ;  //Defines the XMLElement type

void printNode(XMLElement node) {
    cout << "<" << node->name() << endl;

    for (xml_attribute<>* attr = node->first_attribute();
            attr; attr = attr->next_attribute()) {
        cout << attr->name() << "=\"";
        cout << attr->value() << "\"" << endl;
    }

}

void Solver::recurOutput(xml_document<>& doc, vector<string>& ids, XMLElement node) {
    xml_attribute<>* id = node->first_attribute("id");

    if (id != nullptr && vectorContains<string>(ids, id->value())) {
        cerr << "Displaying ID " << id->value() << endl;
        string s;
        int i;

        for (i = 0 ; _shapes[i].getID() != id->value() ; i++);

        array<double, 6> m = _shapes[i].getTransMatrix();
        s = "matrix(";

        for (int e = 0 ; e < 6 ; e++) {
            s += to_string(m[e]) + (e == 5 ? ")" : ", ");
        }

        //char* cs = new char[s.size() + 1];
        char cs[1000];
        strcpy(cs, s.c_str());
        cs[s.size()] = '\0';
        xml_attribute<>* mat = doc.allocate_attribute("transform", cs);
        node->prepend_attribute(mat);
    }
    else if (id != nullptr) {
        cerr << "Ignored ID " << id->value() << endl;
    }

    printNode(node);
    XMLElement next = node->first_node();

    if (next != nullptr) {
        cout << ">" << endl;

        if (node->value_size() == 0)
            for (; next != nullptr ; next = next->next_sibling()) {
                recurOutput(doc, ids, next);
            }
        else {
            cout << node->value() << endl;
        }

        cout << "</" << node->name() << ">" << endl;
    }
    else {
        cout << "/>" << endl;
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
    recurOutput(doc, ids, rootNode);
    return "";
}

string Solver::debugOutputSVG() {
    //Addto osef
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
