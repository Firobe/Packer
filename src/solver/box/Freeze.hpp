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
    
    void randomStartingPoint(Shape shape);
    void FreezeSolver::gravityMoveDown(Shape shape);
    void FreezeSolver::wiggle(Shape shape);
    void FreezeSolver::bounceUp(Shape shape);
    void FreezeSolver::bounceLeft(Shape shape);
    void FreezeSolver::bounceRight(Shape shape);
    void FreezeSolver::bounceShape(Shape shapeMove, Shape shapeStill);
    
public:
    Scanline(std::vector<Shape>& v, std::vector<Parameter> p) : Solver(v, p),
        _boxes(v.size()) {}
};

#endif
