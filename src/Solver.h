#ifndef SOLVER__H
#define SOLVER__H

#include <string>

#include "Shape.h"


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
    std::string outputSVG();
    std::string debugOutputSVG();
};

#endif
