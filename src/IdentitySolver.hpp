#ifndef IDENTITY__H
#define IDENTITY__H

#include "Solver.hpp"

/**
 * Identity solver. Do nothing.
 * Meant as a test solver.
 */
class IdentitySolver : public Solver {
public:
  IdentitySolver(std::vector<Shape>& v, double w, double h) : Solver(v,w,h) {}
    void solve() {}
};

#endif
