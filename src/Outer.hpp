#ifndef OUTER__H
#define OUTER__H

#include <rapidxml_ns/rapidxml_ns.hpp>
#include <rapidxml_ns/rapidxml_ns_utils.hpp>
#include <string>
#include <sstream>

#include "Shape.hpp"

enum NodeType { hasChild, hasValue, noChild };

/**
 * Solver class. An abstract class
 * as a base for solver algorithms.
 *
 * An inherited algorithm only has to
 * implement the solve() method, which
 * should modify _shapes and nothing
 * else.
 */
class Outer {
protected:
    std::vector<Shape>& _shapes;
    std::stringstream _outStream;
    rapidxml_ns::xml_document<> _doc;
    std::vector<std::string>& _ids;
    bool _addTo;
    rapidxml_ns::file<> _svgFile;
    double _height;
public:
    Outer(std::string path, bool addto, std::vector<std::string>& tp, double height,
          std::vector<Shape>&);
    ~Outer();
    static std::string String(std::string path, bool addto, std::vector<std::string>& tp, double height,
          std::vector<Shape>&);
private:
    void recurOutput(rapidxml_ns::xml_node<>*, bool = true);
    void printNode(rapidxml_ns::xml_node<>*);
    NodeType identNode(rapidxml_ns::xml_node<>*);
    bool appendMatrix(rapidxml_ns::xml_node<>*, char*, bool);
};

#endif
