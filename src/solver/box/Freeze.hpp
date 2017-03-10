#ifndef FREEZE__H
#define FREEZE__H

#include <vector>

#include "Solver.hpp"

class Freeze : public Solver {
private:
	void preSolve() override; 
    void solveBin() override;
    
    void awayStartingPoint(Shape& shape);
    void randomStartingPoint(Shape& shape);
    void gravityMoveDown(Shape& shape);
    void wiggle(Shape& shape);
    void bounceUp(Shape& shape);
    void bounceLeft(Shape& shape);
    void bounceRight(Shape& shape);
    void bounceShape(Shape& shapeMove, Shape& shapeStill);
    
public:
    Freeze(std::vector<Shape>& v, std::vector<Parameter> p) : Solver(v, p) {}
};

#endif
