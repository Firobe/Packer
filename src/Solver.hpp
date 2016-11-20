#ifndef SOLVER__H
#define SOLVER__H

#include <rapidxml_ns/rapidxml_ns.hpp>
#include <string>
#include <algorithm>
#include <sstream>


#include "Shape.hpp"


/**
 * Solver class. An abstract class
 * as a base for solver algorithms.
 *
 * An inherited algorithm only has to
 * implement the solve() method, which
 * should modify _shapes and nothing
 * else.
 */
class Solver {
protected:
    std::vector<Shape>& _shapes;
public:
    Solver(std::vector<Shape>& s) : _shapes(s) {}
    void solve() = delete;
    std::string outputSVG(std::string, bool, std::vector<std::string>);
    std::string debugOutputSVG();
private:
    void recurOutput(std::stringstream&, rapidxml_ns::xml_document<>& doc,
                     std::vector<std::string>& ids,
                     rapidxml_ns::xml_node<>* node); //See the implementation
};

/**
 * Utilitary function to check if a vector contains an element.
 */
template<typename T>
bool vectorContains(std::vector<T>& v, T element) {
    return std::find(v.begin(), v.end(), element) != v.end();
}

#endif
