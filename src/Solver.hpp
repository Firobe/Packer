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
    int _binNumber;
    Point _dimensions;
public:
    Solver(std::vector<Shape>& s, Point p) : _shapes(s), _binNumber(0), _dimensions(p) {}
    void solve() = delete;
    std::string debugOutputSVG();
};

#endif
