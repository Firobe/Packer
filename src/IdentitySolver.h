#ifndef IDENTITY__H
#define IDENTITY__H

#include "Solver.h"

/**
 * Identity solver. Do nothing.
 * Meant as a test solver.
 */
class IdentitySolver : public Solver {
public:
    IdentitySolver(std::vector<Shape>& v) : Solver(v) {}
    void solve() {}
};

#endif
