#ifndef TOINFINITYANDBEYOND__H
#define TOINFINITYANDBEYOND__H

#include "Solver.hpp"

/**
 * Very basic solver. Uses bounding boxes
 * to position the shapes on a line (can be a very long one),
 * with the only guaranty being that
 * the shape do not intersect.
 */
class ToInfinityAndBeyond : public Solver {
private:
    void solveBin() override;

public:
    ToInfinityAndBeyond(std::vector<Shape>& v, const std::vector<Parameter>& p) : Solver(v,
                p) {}
};

#endif
