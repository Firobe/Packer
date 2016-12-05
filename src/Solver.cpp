#include <boost/geometry/io/svg/svg_mapper.hpp>
#include <sstream>
#include <string>

#include "Solver.hpp"
#include "Log.hpp"

using namespace std;

/**
 * Output function using the svg output methods of BOOST.
 * Should be used for debug only.
 * Outputs what the solver actually sees.
 */
string Solver::debugOutputSVG() const {
    stringstream ret;
    bg::svg_mapper<Point> mapper(ret, 800, 800);

    for (Shape& s : _shapes) {
        mapper.add(s.getMultiP());
    }

    for (Shape& s : _shapes) {
        mapper.map(s.getMultiP(), "fill:rgb(" + to_string(rand() % 256) + "," +
                   to_string(rand() % 256) + "," + to_string(rand() % 256) + ")");
    }

    LOG(info) << "Debug SVG generated" << endl;
    return ret.str() + "</svg>";
}

void Solver::solve() {
    LOG(info) << "Packing shapes...";
    _indices.clear();

    for (unsigned i = 0; i < _shapes.size(); i++) {
        _indices.push_back(i);
    }

    //_indices is now initialized to {1, 2, ... _shapes.size() - 1 }
    preSolve();

    //While there are shapes left, fill bins individually
    while (!_indices.empty()) {
        LOG(info) << endl << "Bin " << _binNumber + 1
                  << " : " << _indices.size() << " shapes left...";
        solveBin();
        _binNumber++;
    }

    LOG(info) << endl << "Successfully packed "
              << _shapes.size() << " shapes in " << _binNumber << " bins." << endl;
}

