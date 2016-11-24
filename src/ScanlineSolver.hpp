#ifndef SCANLINESOLVER__H
#define SCANLINESOLVER__H

#include "Solver.hpp"
#include <vector>

class ScanlineSolver : public Solver {
public:
    ScanlineSolver(std::vector<Shape>& v, Point p) : Solver(v, p) {}
    void solve();
    void solveAux(std::vector<Box>, std::vector<unsigned>);
};

double getLenFromIndex(std::vector<double>, unsigned);
bool allCellsEmpty(std::vector<std::vector<bool>>, unsigned, int, unsigned, int);
int getLastY(std::vector<double>, unsigned, double, double&);
int getLastX(std::vector<double>, unsigned, double, double&);
void printAll(std::vector<std::vector<bool>>, std::vector<double>, std::vector<double>);

#endif
