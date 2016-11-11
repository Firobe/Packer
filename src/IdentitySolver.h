#ifndef IDENTITY__H
#define IDENTITY__H

#include "Solver.h"

class IdentitySolver : public Solver {
public:
    IdentitySolver(std::vector<Shape>& v) : Solver(v) {}
    void solve() {}
};

#endif
