#ifndef MULTILINESOLVER__H
#define MULTILINESOLVER__H

#include "Solver.hpp"
#include "CloseEnough.hpp"
#include "Log.hpp"

/**
 * Small improvement of LineSolver
 * uses bounding box to place shapes on defined rectangular areas,
 * with the only guaranty being that the shape do not intersect
 * themselves or the areas.
 */
class MultilineSolver : public Solver {
private:
    std::vector<Box> _boxes;
    bool _sort;
    void preSolve() override;
    void solveBin() override;
public:
    MultilineSolver(Layout& v, const std::vector<Parameter>& p) : Solver(v, p),
        _boxes(v.size()) {
		int tmpSort = 1;
        getParameter(p, "sort", tmpSort);
		_sort = tmpSort == 1;
    }
};

#endif
