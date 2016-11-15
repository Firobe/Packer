#ifndef TOINFINITYANDBEYOND__H
#define TOINFINITYANDBEYOND__H

#include "Solver.h"

/**
 * Very basic solver. Uses bounding boxes
 * to position the shapes on a line (can be a very long one),
 * with the only guaranty being that
 * the shape do not intersect.
 */
class ToInfinityAndBeyondSolver : public Solver {
public:
    ToInfinityAndBeyondSolver(std::vector<Shape>& v) : Solver(v) {}
    void solve();
};

#endif
