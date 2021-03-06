#include <sstream>
#include <string>

#include <rapidxml_ns/rapidxml_ns_print.hpp>
#include <rapidxml_ns/rapidxml_ns_utils.hpp>
#include <boost/geometry/io/svg/svg_mapper.hpp>

#include "Outer.hpp"
#include "Matrix.hpp"
#include "solver/Solver.hpp"
#include "Log.hpp"

using namespace std;
using namespace rapidxml_ns;
using XMLElement = rapidxml_ns::xml_node<>* ;  //Defines the XMLElement type

Outer::Outer(const std::string& path, bool addto, std::vector<std::string>& tp,
             Layout& s) :
    _shapes(s),
    _ids(tp),
    _addTo(addto),
    _svgFile(path.c_str()),
    _currentShape(-1),
    _stopPrinting(false) {
    //Opening SVG file
    _doc.parse<0>(_svgFile.data());
}

Outer::~Outer() {
    //Exists for the sake of _svgFile
    //Needed for attributes destructors to be called automatically
}

/**
 * Writes to ss the content of
 * a node : the beginning the tag, the name
 * and each of its attributes
 */
void Outer::printNode(XMLElement node, bool forceNoMatrix) {
    stringstream tmp;
    tmp << "<" << node->name() << "\n";
    LOG(trace) << "Printing " << node->name() << endl;

    // For each attribute to write within the node
    for (xml_attribute<>* attr = node->first_attribute();
            attr; attr = attr->next_attribute()) {
        tmp << attr->name() << "=\"";

        if (strcmp(attr->name(), "id") == 0) {
            //adding bool "forceNoMatrix" (0/1) to avoid duplication of attribute identifiers
            tmp << attr->value() << forceNoMatrix << "\"\n";
        }
        else
            tmp << attr->value() << "\"\n";
    }

    if (_currentShape == -1 || forceNoMatrix) {
        if (forceNoMatrix || (!_stopPrinting && strcmp(node->name(), "g") != 0)) {
            //Stop printing upon encountering the first useless group
            cout << tmp.str();
        }
        else
            _stopPrinting = true;
    }
    else
        _shapes[_currentShape].appendOut(tmp.str());
}

/**
 * Returns the index in _shapes
 * corresponding to the current node
 * by checking the IDs
 */
int Outer::getCurrentShape(XMLElement node) const {
    //Check if that ID exists and is in our packed list
    xml_attribute<>* id = node->first_attribute("id");

    // Case id error/not found
    if (id == nullptr || !vectorContains<string>(_ids, id->value()))
        return -1;

    //Find to which shape (i) the ID belongs to
    for (unsigned i = 0 ; i < _shapes.size() ; ++i)
        if (_shapes[i].getIdentifier() == id->value())
            return i;

    return -1;
}

/**
 * Appends the correct transformation matrix
 * to the attributes of a node.
 * Returns true if it should append the matrix
 * Does not effectively append it if forceNoMatrix == true
 */
bool Outer::appendMatrix(XMLElement node, char*& cs, bool forceNoMatrix) {
    int i = getCurrentShape(node);

    if (i == -1)
        return false;

    //Used for duplication
    if (forceNoMatrix)
        return true;

    //Get the matrix and write its SVG string equivalent
    array<double, 6> m = _shapes[i].getTransMatrix();
    m[1] *= -1;
    m[2] *= -1;

    if (_addTo)
        m[5] += Parser::getDims().y();

    // string equivalent of the matrix
    stringstream s;
    s << Matrix(m);
    // transform attribute
    xml_attribute<>* transAtt = node->first_attribute("transform");

    if (transAtt != nullptr) {
        // old transform matrices found : add them with the new one
        if (strcmp(node->name(), "g") != 0) { //Only add applied matrices (element ones)
            s << " ";
            s << transAtt->value();
        }

        node->remove_attribute(transAtt);
    }

    LOG(trace) << "Appending " << s.str() << endl;
    //Allocate space and add it as an attribute
    cs = new char[s.str().size() + 1];
    strcpy(cs, s.str().c_str());
    cs[s.str().size()] = '\0';
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
 * duplicates a shape by calling itself again with forceNoMatrix = false
 */
void Outer::recurOutput(XMLElement node, bool forceNoMatrix) {
    bool computed = (_currentShape == -1);
    stringstream outStream;
    LOG(trace) << "Current shape : " << _currentShape << "/" << _shapes.size() << endl;

    if (computed)
        _currentShape = getCurrentShape(node);

    //Get the ID attribute of the node
    char* cs = nullptr;
    bool packed = appendMatrix(node, cs, forceNoMatrix);
    printNode(node, forceNoMatrix);
    delete[] cs;

    //Get the first child of the node
    switch (identNode(node)) {
    case noChild: // no node within
        outStream << "/>\n";
        break;

    case hasChild: { // node(s) within
        if (_currentShape == -1 || forceNoMatrix) {
            if (!_stopPrinting || forceNoMatrix)
                cout << ">\n";
        }
        else
            _shapes[_currentShape].appendOut(">\n");

        //Call each of the children
        XMLElement next = node->first_node();

        for (; next != nullptr ; next = next->next_sibling()) {
            //Propagate the forceNoMatrix if already packed
            recurOutput(next, (packed and !forceNoMatrix) ? false : _addTo);
        }

        //Do not close the file yet, wait for groupShapes
        if (strcmp(node->name(), "svg") != 0)
            outStream << "</" << node->name() << ">\n";

        break;
    }

    case hasValue: // has node within : only a value
        outStream << ">" << node->value() << "</" << node->name() << ">\n";
        break;
    }

    //Display the cached output in cout or store it in the correct shape
    if (_currentShape == -1 || forceNoMatrix) {
        if (!_stopPrinting || forceNoMatrix)
            cout << outStream.str();
    }
    else
        _shapes[_currentShape].appendOut(outStream.str());

    //If the currentShape was -1, set it back
    if (computed)
        _currentShape = -1;

    //Duplicate if needed
    if (packed && forceNoMatrix)
        recurOutput(node, false);
}

/**
 * Check the children of a node : if it has a child (and check
 * if that child is a value).
 */
NodeType Outer::identNode(XMLElement node) const {
    XMLElement next = node->first_node();

    if (next != nullptr) {
        if (node->value_size() == 0)
            return hasChild;
        else
            return hasValue;
    }
    else
        return noChild;
}

/**
 * Now that the shapes are loaded with
 * their out string, display them
 * in group, corresponding to the bins.
 */
void Outer::groupShapes() {
    //Sort shapes by final height of first point
    sort(_shapes.begin(), _shapes.end(), [](const Shape & a, const Shape & b) {
        return a.getNthPoly(0).outer()[0].y() <
               b.getNthPoly(0).outer()[0].y();
    });
    unsigned i = 0;
    double curBottom = 0;

    //Loop on bins
    while (i < _shapes.size()) {
        cout << "<g>" << endl;

        //Loop on elements of a single bin
        while (i < _shapes.size() and
                (_shapes[i].getNthPoly(0).outer()[0].y() - curBottom) < Parser::getDims().y()) {
            cout << _shapes[i].getOut();
            ++i;
        }

        curBottom += Parser::getDims().y() * SPACE_COEF;
        cout << "</g>" << endl;
    }

    //End the document once and for all
    cout << "</svg>" << endl;
}

/**
 * Returns a string corresponding to the SVG file to ouput.
 * (Theoretically with the packed shapes)
 */
void Outer::Write(const std::string& path, bool addto, std::vector<std::string>& tp,
                  Layout& s) {
    Outer outer(path, addto, tp, s);
    XMLElement rootNode = outer._doc.first_node();
    LOG(info) << "Producing SVG output... (addto=" << addto << ")\n";
    outer.recurOutput(rootNode, addto);
    LOG(info) << "Original file completely parsed..." << endl;
    outer.groupShapes();
    LOG(info) << "SVG successfully generated." << endl;
}

/**
 * Computes the compression ratio :
 * actual area / minimal area
 */
double compressionRatio(Layout& _shapes) {
    //Computing the total enveloppe of shapes
    std::vector<Box> boxes(_shapes.size());

    for (unsigned i = 0; i < _shapes.size(); i++)
        _shapes[i].envelope(boxes[i]);

    //Max x-axis point
    auto xIt = max_element(boxes.begin(), boxes.end(),
    [](Box & a, Box & b) {
        return a.max_corner().x() < b.max_corner().x();
    });
    //Max y-axis point
    auto yIt = max_element(boxes.begin(), boxes.end(),
    [](Box & a, Box & b) {
        return a.max_corner().y() < b.max_corner().y();
    });
    int binNumber = yIt->max_corner().y() / Parser::getDims().y();
    //Compensate spacing between bins
    Point maxCorner((*xIt).max_corner().x(),
                    (*yIt).max_corner().y() - (binNumber - 1) * Parser::getDims().y() * (SPACE_COEF - 1));
    LOG(debug) << "Max point is (" << maxCorner.x() << ", " << maxCorner.y() << ")\n";
    LOG(debug) << "(total area : " << maxCorner.x() * maxCorner.y() << endl;
    //Computing the sum of every shape area
    double sum = 0.;

    for (Shape& s : _shapes)
        sum += s.area();

    LOG(debug) << "Minimal area is " << sum << endl;
    //Computing ratio
    return (maxCorner.x() * maxCorner.y()) / sum;
}

/**
 * Output function using the svg output methods of BOOST.
 * Should be used for debug only.
 * Outputs what the solver actually sees.
 */
string debugOutputSVG(Layout& _shapes) {
    stringstream ret;
    bg::svg_mapper <Point> mapper(ret, Parser::getDims().x(), Parser::getDims().y());

    for (const Shape& s : _shapes) {
        int r = rand() % 256, g = rand() % 256, b = rand() % 256;

        for (unsigned i = 0 ; i < s.polyNumber() ; ++i) {
            mapper.add(s.getNthPoly(i));
            mapper.map(s.getNthPoly(i), "fill:rgb(" + to_string(r) + "," +
                       to_string(g) + "," + to_string(b) + ")");
        }
    }

    LOG(info) << "Debug SVG generated" << endl;
    return ret.str() + "</svg>";
}
