#ifndef FREEZE__H
#define FREEZE__H

#include <vector>

#include "Solver.hpp"


/**
 * @brief Solver simulating gravity and random bounces on shape collision
 */
class FreezeSolver : public Solver {
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

public:
    FreezeSolver(Layout& v, const std::vector<Parameter>& p) : Solver(v, p) {}
};

#endif
