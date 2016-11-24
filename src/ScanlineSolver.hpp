#ifndef SCANLINESOLVER__H
#define SCANLINESOLVER__H

#include "Solver.hpp"
#include <vector>
#include <list>

class ScanlineSolver : public Solver {
private:
	std::vector<Box> _boxes;
	std::list<unsigned> _indices;
    void solveAux();
public:
    ScanlineSolver(std::vector<Shape>& v, Point p) : Solver(v, p),
   		_boxes(v.size()) {}
    void solve();
};

double getLenFromIndex(std::vector<double>&, unsigned);
bool allCellsEmpty(std::vector<std::vector<bool>>&, unsigned, int, unsigned, int);
int getLastY(std::vector<double>&, unsigned, double, double&);
int getLastX(std::vector<double>&, unsigned, double, double&);

#endif
