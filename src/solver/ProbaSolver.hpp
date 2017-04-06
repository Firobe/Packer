#ifndef PROBASOLVER__H
#define PROBASOLVER__H

#define DEFAULT_AMPLITUDE (10.)
#define DEFAULT_RANDOM (50)
#define DEFAULT_BRANCH (50)

#include "Solver.hpp"
#include "CloseEnough.hpp"

/**
 * @brief Solver exploiting random steps
 * @param random_steps The number of completely random steps per branch
 * @param branch_steps The number of branches to try
 * @param amplitude_proba An indicator of the probability of a shape doing wild movements
 * The higher the slower
 */
class ProbaSolver : public Solver {
private:
    double _amplitudeProba;
    bool _initialPlacement;
    std::vector<Point> _centroids;
    std::vector<unsigned> _binNbs;
	int _randomSteps;
	int _branchSteps;
    void preSolve() override;
    void solveBin() override;

    void nextStep();
    double RNG() const;
    bool shapeInBin(unsigned, int = -1) const;
public:
    ProbaSolver(Layout& v, const std::vector<Parameter>& p) : Solver(v, p) {
        if (!getParameter(p, "random_steps", _randomSteps))
            _randomSteps = DEFAULT_RANDOM;

        if (!getParameter(p, "branch_steps", _branchSteps))
            _branchSteps = DEFAULT_BRANCH;

        if (!getParameter(p, "amplitude_proba", _amplitudeProba))
            _amplitudeProba = DEFAULT_AMPLITUDE;

        int tmpPlace = 1;
        getParameter(p, "initial_placement", tmpPlace);
        _initialPlacement = tmpPlace;
    }
};

#endif
