#ifndef TOINFINITYANDBEYOND__H
#define TOINFINITYANDBEYOND__H

#include "Solver.h"

class ToInfinityAndBeyondSolver : public Solver {
public:
 ToInfinityAndBeyondSolver(std::vector<Shape>& v) : Solver(v) {}
  void solve();
};

#endif
