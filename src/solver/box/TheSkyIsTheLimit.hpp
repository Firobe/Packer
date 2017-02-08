#ifndef THESKYISTHELIMIT__H
#define THESKYISTHELIMIT__H

#include "Solver.hpp"

/**
 * Small improvement of ToInfinityAndBeyond solver
 * uses bounding box to place shapes on defined rectangular areas,
 * with the only guaranty being that the shape do not intersect
 * themselves or the areas.
 */
class TheSkyIsTheLimit : public Solver {
private:
    std::vector<Box> _boxes;

    void preSolve() override;

    void solveBin() override;

public:
    TheSkyIsTheLimit(std::vector<Shape>& v) : Solver(v), _boxes(v.size()) {}
};

#endif
