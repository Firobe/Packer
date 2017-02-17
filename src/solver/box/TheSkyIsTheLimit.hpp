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
    TheSkyIsTheLimit(std::vector<Shape>& v, std::vector<Parameter> p) : Solver(v, p),
        _boxes(v.size()) {}
};

#endif
