#ifndef IDENTITY__H
#define IDENTITY__H

#include "Solver.hpp"

/**
 * Identity solver. Do nothing.
 * Meant as a test solver.
 */
class IdentitySolver : public Solver {
public:
    IdentitySolver(std::vector<Shape>& v, Point p = Point(0, 0)) : Solver(v, p) {}
    void solve() {}
};

#endif
