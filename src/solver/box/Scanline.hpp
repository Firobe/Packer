#ifndef SCANLINESOLVER__H
#define SCANLINESOLVER__H

#include <vector>

#include "Solver.hpp"

class Scanline : public Solver {
private:
    std::vector<Box> _boxes;
    double getLenFromIndex(const std::vector<double>&, unsigned) const;
    bool allCellsEmpty(const std::vector<std::vector<bool>>&, unsigned, int, unsigned,
                       int) const;
    int getLast(const std::vector<double>&, unsigned, double, double&) const;
    void preSolve() override;
    void solveBin() override;
    void printAll(std::vector<std::vector<bool>>& cellIsEmpty, std::vector<double> cellW,
                  std::vector<double>& cellH);
public:
    Scanline(std::vector<Shape>& v, std::vector<Parameter> p) : Solver(v, p),
        _boxes(v.size()) {}
};

#endif
