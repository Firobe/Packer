#include <sstream>
#include <string>
#include <numeric>

#include <boost/geometry/io/svg/svg_mapper.hpp>
#include <boost/geometry/algorithms/envelope.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/strategies/cartesian/area_surveyor.hpp>

#include "Solver.hpp"
#include "Log.hpp"

using namespace std;

void Solver::solve() {
    LOG(info) << "Packing shapes..." << endl;
    _indices.resize(_shapes.size());
    iota(_indices.begin(), _indices.end(), 0);
    //_indices is now initialized to {1, 2, ... _shapes.size() - 1 }
    LOG(info) << "Pre-solving..." << endl;
    preSolve();
    LOG(info) << "Solving..." << endl;

    //While there are shapes left, fill bins individually
    while (!_indices.empty()) {
        LOG(info) << "Bin " << _binNumber + 1
                  << " : " << _indices.size() << " shapes left..." << endl;
        solveBin();
        _binNumber++;
    }

    LOG(info) << "Successfully packed "
              << _shapes.size() << " shapes in " << _binNumber << " bins." << endl;
}

/**
 * Computes the compression ratio :
 * actual area / minimal area
 */
double Solver::compressionRatio() const {
    //Computing the total enveloppe of shapes
    std::vector<Box> boxes(_shapes.size());

    for (unsigned i = 0; i < _shapes.size(); i++) {
        bg::envelope(_shapes[i].getMultiP(), boxes[i]);
    }

    //Max x-axis point
    auto xIt = max_element(boxes.begin(), boxes.end(),
                           [](Box &a, Box &b) {
                               return a.max_corner().x() < b.max_corner().x();
                           });
    //Max y-axis point
    auto yIt = max_element(boxes.begin(), boxes.end(),
                           [](Box &a, Box &b) {
                               return a.max_corner().y() < b.max_corner().y();
                           });
    //Compensate spacing between bins
    Point maxCorner((*xIt).max_corner().x(),
                    (*yIt).max_corner().y() - (_binNumber - 1) * _dimensions.y() * (SPACE_COEF - 1));
    LOG(debug) << "Max point is (" << maxCorner.x() << ", " << maxCorner.y() << ")\n";
    LOG(debug) << "(total area : " << maxCorner.x() * maxCorner.y() << endl;
    //Computing the sum of every shape area
    double sum = 0.;

    for (auto && s : _shapes) {
        sum += bg::area(s.getMultiP());
    }

    LOG(debug) << "Minimal area is " << sum << endl;
    //Computing ratio
    return (maxCorner.x() * maxCorner.y()) / sum;
}

/**
 * Output function using the svg output methods of BOOST.
 * Should be used for debug only.
 * Outputs what the solver actually sees.
 */
string Solver::debugOutputSVG() const {
    stringstream ret;
    bg::svg_mapper <Point> mapper(ret, 800, 800);

    for (const Shape &s : _shapes) {
        mapper.add(s.getMultiP());
    }

    for (const Shape &s : _shapes) {
        mapper.map(s.getMultiP(), "fill:rgb(" + to_string(rand() % 256) + "," +
                                  to_string(rand() % 256) + "," + to_string(rand() % 256) + ")");
    }

    LOG(info) << "Debug SVG generated" << endl;
    return ret.str() + "</svg>";
}