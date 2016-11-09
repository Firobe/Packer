#ifndef SOLVER__H
#define SOLVER__H

#include <boost/geometry.hpp>
#include <string>
#include "Shape.h"

class Solver {
protected:
    std::vector<Shape>& _shapes;
public:
    Solver(std::vector<Shape>& s) : _shapes(s) {}
    void solve() = delete;
    std::string outputSVG();
};

#endif
