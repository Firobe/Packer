#ifndef PROBASOLVER__H
#define PROBASOLVER__H

#define DEFAULT_AMPLITUDE (0.01)

#include "Solver.hpp"
#include "CloseEnough.hpp"

class ProbaSolver : public Solver {
private:
    double _amplitudeProba;
	std::vector<Point> _centroids;
    void preSolve() override;
    void solveBin() override;

	void nextStep();
	double swagRNG() const;
public:
    ProbaSolver(Layout& v, const std::vector<Parameter>& p) : Solver(v, p) {
        if (!getParameter(p, "amplitudeProba", _amplitudeProba))
            _amplitudeProba = DEFAULT_AMPLITUDE;
    }
};

#endif
