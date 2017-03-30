#ifndef PROBASOLVER__H
#define PROBASOLVER__H

#define DEFAULT_AMPLITUDE (10.)

#include "Solver.hpp"
#include "CloseEnough.hpp"
#include "Matrix.hpp"

struct Solution {
    std::vector<Matrix> mats;
    double quality;
};

class ProbaSolver : public Solver {
private:
    double _amplitudeProba;
    bool _initialPlacement;
    std::vector<Point> _centroids;
    std::vector<unsigned> _binNbs;
    void preSolve() override;
    void solveBin() override;

    void nextStep();
    double swagRNG() const;
    bool shapeInBin(unsigned, int = -1) const;
    void applySolution(Solution& s);
    void genSolution(Solution& s) const;
    double quality() const;
public:
    ProbaSolver(Layout& v, const std::vector<Parameter>& p) : Solver(v, p) {
        if (!getParameter(p, "amplitude_proba", _amplitudeProba))
            _amplitudeProba = DEFAULT_AMPLITUDE;

        int tmpPlace = 1;
        getParameter(p, "initial_placement", tmpPlace);
        _initialPlacement = tmpPlace;
    }
};

#endif
