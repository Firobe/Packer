#include <algorithm>
#include <random>
#include <thread>
#include <chrono>

#include <boost/geometry/algorithms/distance.hpp>

#include "ProbaSolver.hpp"
#include "Shape.hpp"
#include "MultilineSolver.hpp"
#include "Display.hpp"

using namespace std;

void ProbaSolver::preSolve() {
    if (_initialPlacement) {
        random_shuffle(_shapes.begin(), _shapes.end());
        MultilineSolver s(_shapes, {{"sort", 0}});
        s.solve();
    }

    _centroids.reserve(_shapes.size());

    for (Shape& s : _shapes) {
        _centroids.emplace_back(s.centroid());
        Box e;
        s.envelope(e);
        _binNbs.emplace_back(e.min_corner().y() / (Parser::getDims().y() * SPACE_COEF));
        _binNumber = max(_binNumber, _binNbs.back());
    }

    Display::Reset();
}

void ProbaSolver::solveBin() {
    Solution current;
    genSolution(current);
    LOG(info) << "Doing random steps";

    for (int i = 0 ; i < 50 ; ++i) {
        for (unsigned step = 0 ; step < 20 ; ++step)
            nextStep();

        Solution newS;
        genSolution(newS);

        if (newS.quality >= current.quality) {
            Display::Text("Iteration " + to_string(i) + " discarded");
            applySolution(current);
            LOG(info) << ".";
        }
        else {
            Display::UpdateAll();
            Display::Text("Iteration " + to_string(i) + " accepted");
            current = newS;
            LOG(info) << "!";
        }
    }

    LOG(info) << endl;
    _indices.clear();
}

double ProbaSolver::swagRNG() const {
    static default_random_engine generator;
    static exponential_distribution<double> distribution(_amplitudeProba);
    return distribution(generator);
}

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
 * Next iteration
 */
void ProbaSolver::nextStep() {
    for (unsigned i = 0 ; i < _shapes.size() ; ++i) {
        double angle = (1 - (rand() % 2) * 2) * swagRNG() * 100;
        double tX = (0.9 - (rand() % 2) * 2) * swagRNG() * Parser::getDims().x() / 10,
               tY = (0.9 - (rand() % 2) * 2) * swagRNG() * Parser::getDims().y() / 10;
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

double ProbaSolver::quality() const {
    double sum = 0;

    for (unsigned i = 0 ; i < _shapes.size() ; ++i) {
        Point orig(0, Parser::getDims().y() * SPACE_COEF * _binNbs[i]);
        double d = bg::distance(orig, _centroids[i]);
        sum += d * d;
    }

    return sum;
}

void ProbaSolver::genSolution(Solution& s) const {
    s.mats.clear();
    s.mats.reserve(_shapes.size());

    for (Shape& sh : _shapes)
        s.mats.push_back(sh.getTransMatrix());

    s.quality = quality();
}

void ProbaSolver::applySolution(Solution& s) {
    for (unsigned i = 0 ; i < _shapes.size() ; ++i) {
        _shapes[i].restore();
        _shapes[i].applyMatrix(s.mats[i]);
    }
}
