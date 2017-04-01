#include <algorithm>
#include <random>
#include <thread>
#include <chrono>
#include <iostream>

#include <boost/geometry/algorithms/distance.hpp>

#include "ProbaSolver.hpp"
#include "Shape.hpp"
#include "MultilineSolver.hpp"
#include "Display.hpp"

using namespace std;

void ProbaSolver::preSolve() {
	//If the user did not prohibit it, use MultilineSolver to place the shapes
	//such that they do not intersect
    if (_initialPlacement) {
		//Random initial order
        random_shuffle(_shapes.begin(), _shapes.end());
        MultilineSolver s(_shapes, {{"sort", 0}});
        s.solve();
    }

    _centroids.reserve(_shapes.size());

    for (Shape& s : _shapes) {
		//Cache the centroids of the shape
        _centroids.emplace_back(s.centroid());
        Box e;
        s.envelope(e);
		//Cache the bin number of each shape
        _binNbs.emplace_back(e.min_corner().y() / (Parser::getDims().y() * SPACE_COEF));
        _binNumber = max(_binNumber, _binNbs.back());
    }

    Display::Reset();
}

void ProbaSolver::solveBin() {
	//Save the current state
    Solution current;
    genSolution(current);
    LOG(info) << "Doing random steps";

    for (int i = 0 ; i < _branchSteps ; ++i) {
		//Do random steps
        for (int step = 0 ; step < _randomSteps ; ++step)
            nextStep();

		//Evaluate the quality of the resulting solution
        Solution newS;
        genSolution(newS);

        if (newS.quality >= current.quality) {
			//Restore old state if not improving
            Display::Text("Iteration " + to_string(i) + " discarded");
            applySolution(current);
            LOG(info) << ".";
        }
        else {
			//Replace old state by new one if improving
            Display::UpdateAll();
            Display::Text("Iteration " + to_string(i) + " accepted");
            current = newS;
            LOG(info) << "!";
        }
    }

    LOG(info) << endl;
    _indices.clear();
}

/**
 * @brief Random number generator
 * Uses an exponential distribution
 */
double ProbaSolver::RNG() const {
    static default_random_engine generator;
    static exponential_distribution<double> distribution(_amplitudeProba);
    return distribution(generator);
}

/**
 * @brief Check that a shape at index i is in its own bin
 * If binNb is specified, check for that bin
 * @param i
 * @param binNb
 */
bool ProbaSolver::shapeInBin(unsigned i, int binNb) const {
    if (binNb == -1)
        binNb = _binNbs[i];

    Box e;
    _shapes[i].envelope(e);
    double minHeight = Parser::getDims().y() * SPACE_COEF * binNb;
    return e.min_corner().x() >= 0 and e.max_corner().x() <= Parser::getDims().x()
           and e.min_corner().y() >= minHeight and
           e.max_corner().y() <= minHeight + Parser::getDims().y();
}

/**
 * @brief Next iteration
 * Moves each shape with a random translation/rotation
 * and checks that is does not intersect with other (cancel the movement if it does)
 */
void ProbaSolver::nextStep() {
    for (unsigned i = 0 ; i < _shapes.size() ; ++i) {
        double angle = (1 - (rand() % 2) * 2) * RNG() * 100;
        double tX = (0.9 - (rand() % 2) * 2) * RNG() * Parser::getDims().x() / 10,
               tY = (0.9 - (rand() % 2) * 2) * RNG() * Parser::getDims().y() / 10;
        _shapes[i].translate(-_centroids[i].x(), -_centroids[i].y());
        _shapes[i].rotate(angle);
        _shapes[i].translate(_centroids[i].x() + tX, _centroids[i].y() + tY);
        bool invalid = !shapeInBin(i);

        for (unsigned j = 0 ; !invalid and j < _shapes.size() ; ++j)
            if (i != j and _binNbs[i] == _binNbs[j] and _shapes[i].intersectsWith(_shapes[j]))
                invalid = true;

        translate(_centroids[i], tX, tY);

        if (invalid) {
            //Cancel movement
            _shapes[i].translate(-_centroids[i].x(), -_centroids[i].y());
            _shapes[i].rotate(-angle);
            _shapes[i].translate(_centroids[i].x() - tX, _centroids[i].y() - tY);
        }
        //this_thread::sleep_for(chrono::milliseconds(2));
    }
}

/**
 * @brief Evaluates the quality of a solution
 * Currently sums the squared distance of each shape
 * to the origin
 */
double ProbaSolver::quality() const {
    double sum = 0;

    for (unsigned i = 0 ; i < _shapes.size() ; ++i) {
        Point orig(0, Parser::getDims().y() * SPACE_COEF * _binNbs[i]);
        double d = bg::distance(orig, _centroids[i]);
        sum += d * d;
    }

    return sum;
}

/**
 * @brief Creates a Solution from the current state
 * (generates every transformation Matrix)
 * @param s Where the solution will be generated
 */
void ProbaSolver::genSolution(Solution& s) const {
    s.mats.clear();
    s.mats.reserve(_shapes.size());

    for (Shape& sh : _shapes)
        s.mats.push_back(sh.getTransMatrix());

    s.quality = quality();
}

/**
 * @brief Apply a solution to the current state
 * (apply every transformation matrix)
 * @param s The solution to apply
 */
void ProbaSolver::applySolution(Solution& s) {
    for (unsigned i = 0 ; i < _shapes.size() ; ++i) {
        _shapes[i].restore();
        _shapes[i].applyMatrix(s.mats[i]);
    }
}
