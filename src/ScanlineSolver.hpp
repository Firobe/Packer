#ifndef SCANLINESOLVER__H
#define SCANLINESOLVER__H

#include "Solver.hpp"
#include <vector>

class ScanlineSolver : public Solver {
private:
    std::vector<Box> _boxes;
    double getLenFromIndex(const std::vector<double>&, unsigned) const;
    bool allCellsEmpty(const std::vector<std::vector<bool>>&, unsigned, int, unsigned,
                       int) const;
    int getLast(const std::vector<double>&, unsigned, double, double&) const;
    void preSolve() override;
    void solveBin() override;
public:
    ScanlineSolver(std::vector<Shape>& v, Point p) : Solver(v, p),
        _boxes(v.size()) {}
};

#endif
