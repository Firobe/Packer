#include <boost/geometry/io/svg/svg_mapper.hpp>
#include <rapidxml_ns/rapidxml_ns_print.hpp>
#include <rapidxml_ns/rapidxml_ns_utils.hpp>
#include <sstream>
#include <string>

#include "Outer.hpp"

using namespace std;
using namespace rapidxml_ns;
using XMLElement = rapidxml_ns::xml_node<>* ;  //Defines the XMLElement type

Outer::Outer(std::string path, bool addto, std::vector<std::string>& tp, double height,
             vector<Shape>& s) :
    _shapes(s),
    _ids(tp),
    _addTo(addto),
    _svgFile(path.c_str()),
    _height(height) {
    //Opening SVG file
    _doc.parse<0>(_svgFile.data());
}

Outer::~Outer() {
    //Exists for the sake of _svgFile
}

/**
 * Writes to ss the content of
 * a node : the beginning the tag, the name
 * and each of its attributes
 */
void Outer::printNode(XMLElement node) {
    _outStream << "<" << node->name() << endl;
    cerr << "Printing " << node->name() << endl;

    for (xml_attribute<>* attr = node->first_attribute();
            attr; attr = attr->next_attribute()) {
        _outStream << attr->name() << "=\"";
        _outStream << attr->value() << "\"" << endl;
    }
}

/**
 * Appends the correct transformation matrix
 * to the attributes of a node.
 * Returns true if it shoud append the matrix
 * Does not effectively append it if forceNoMatrix == true
 */
bool Outer::appendMatrix(XMLElement node, char* cs, bool forceNoMatrix) {
    //Check if that ID exists and is in our packed list
    xml_attribute<>* id = node->first_attribute("id");

    if (id == nullptr || !vectorContains<string>(_ids, id->value())) {
        return false;
    }

    if (forceNoMatrix) {
        return true;
    }

    cerr << "Adding matrix for " << id->value() << endl;
    string s;
    int i;

    //Then creates the matrix and add it as an attribute
    //Find to which shape (i) the ID belongs to
    for (i = 0 ; _shapes[i].getID() != id->value() ; i++);

    //Get the matrix and write its SVG string equivalent
    array<double, 6> m = _shapes[i].getTransMatrix();

    if (_addTo) {
        m[5] += _height;
    }

    s = "matrix(";

    for (int e = 0 ; e < 6 ; e++) {
        s += to_string(m[e]) + (e == 5 ? ")" : ", ");
    }

    xml_attribute<>* transAtt = node->first_attribute("transform");

    if (transAtt != nullptr) {
        s += " ";
        s += transAtt->value();
        node->remove_attribute(transAtt);
    }

    cerr << "Appending " << s << endl;

    //Allocate space and add it as an attribute
    cs = new char[s.size() + 1];
    strcpy(cs, s.c_str());
    cs[s.size()] = '\0';
    xml_attribute<>* mat = _doc.allocate_attribute("transform", cs);
    node->prepend_attribute(mat);
    return true;
}

/**
 * Main output function. Recursive.
 * Display the node (and adds a transformation matrix if the node
 * is a shape which was packed by our algorithm).
 * Then calls itself on the children of the nodes.
 *
 * By default, forceNoMatrix == addTo. If it's true, it potentially
 * duplicate a shape by calling itself again with forceNoMatrix = false
 */
void Outer::recurOutput(XMLElement node, bool forceNoMatrix) {
    //Get the ID attribute of the node
    char* cs = nullptr;
    bool packed = appendMatrix(node, cs, forceNoMatrix);
    printNode(node);
    delete[] cs;

    //Get the first child of the node
    switch (identNode(node)) {
    case noChild:
        _outStream << "/>" << endl;
        break;

    case hasChild: {
        _outStream << ">" << endl;
        //Call each of the children
        XMLElement next = node->first_node();

        for (; next != nullptr ; next = next->next_sibling()) {
            recurOutput(next, _addTo);
        }

        _outStream << "</" << node->name() << ">" << endl;
        break;
    }

    case hasValue:
        _outStream << ">" << node->value() << "</" << node->name() << ">" << endl;
        break;
    }

    //Duplicate if needed
    if (packed && forceNoMatrix) {
        recurOutput(node, false);
    }
}

/**
 * Check the children of a node : if it has a child (and check
 * if that child is a value).
 */
NodeType Outer::identNode(XMLElement node) {
    XMLElement next = node->first_node();

    if (next != nullptr) {
        if (node->value_size() == 0) {
            return hasChild;
        }
        else {
            return hasValue;
        }
    }
    else {
        return noChild;
    }
}

/**
 * Returns a string corresponding to the SVG file to ouput.
 * (Theoretically with the packed shapes)
 */
string Outer::String(std::string path, bool addto, std::vector<std::string>& tp,
                     double height,
                     std::vector<Shape>& s) {
    Outer outer(path, addto, tp, height, s);
    XMLElement rootNode = outer._doc.first_node();
    cerr << "Beginning SVG production (addto=" << addto << ")\n";
    outer.recurOutput(rootNode, addto);
    cerr << "SVG successfully generated" << endl;
    return outer._outStream.str();
}
