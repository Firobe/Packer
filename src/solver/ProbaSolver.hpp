#ifndef PROBASOLVER__H
#define PROBASOLVER__H

#define DEFAULT_AMPLITUDE (0.01)

#include "Solver.hpp"

class ProbaSolver : public Solver {
private:
    double _amplitudeProba;
    void preSolve() override;
    void solveBin() override;
public:
    ProbaSolver(std::vector<Shape>& v, const std::vector<Parameter>& p) : Solver(v, p) {
        if (!getParameter(p, "amplitudeProba", _amplitudeProba))
            _amplitudeProba = DEFAULT_AMPLITUDE;
    }
};

#endif
