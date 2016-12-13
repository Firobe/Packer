#ifndef OUTER__H
#define OUTER__H

#include <sstream>
#include <string>

#include <rapidxml_ns/rapidxml_ns.hpp>
#include <rapidxml_ns/rapidxml_ns_utils.hpp>

#include "Shape.hpp"

// Node from svg tree
enum NodeType { hasChild, hasValue, noChild };

/**
 *Output class
 *Writes the resulting SVG file
 *on the standard output, to be written directly
 *in an SVG file, or transfered to the inkscape plugin
 */
class Outer {
protected:
    std::vector<Shape>& _shapes;
    rapidxml_ns::xml_document<> _doc; //SVG file to parse.
    std::vector<std::string>& _ids; //IDs of packed shapes.
    bool _addTo; //Are we duplicating or not
    rapidxml_ns::file<> _svgFile; //SVG file used to build _doc
    double _height;
public:
    static void Write(std::string path, bool addto, std::vector<std::string>& tp,
                      double height,
                      std::vector<Shape>&);
private:
    Outer(std::string path, bool addto, std::vector<std::string>& tp, double height,
          std::vector<Shape>&);
    ~Outer();
    void recurOutput(rapidxml_ns::xml_node<>*, bool = true);
    void printNode(rapidxml_ns::xml_node<>*, bool);
    NodeType identNode(rapidxml_ns::xml_node<>*) const;
    bool appendMatrix(rapidxml_ns::xml_node<>*, char*&, bool);
};

#endif
