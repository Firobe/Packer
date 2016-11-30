#include <boost/geometry/io/svg/svg_mapper.hpp>
#include <sstream>
#include <string>

#include "Solver.hpp"

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

    cerr << "SVG successfully generated" << endl;
    return ret.str() + "</svg>";
}

void Solver::solve() {
    _indices.clear();

    for (unsigned i = 0; i < _shapes.size(); i++) {
        _indices.push_back(i);
    }
	//_indices is now initialized to {1, 2, ... _shapes.size() - 1 }

    preSolve();

	//While there are shapes left, fill bins individually
    while (!_indices.empty()) {
        cerr << _indices.size() << " shapes left..." << endl;
        solveBin();
        _binNumber++;
    }

    cerr << "Successfully packed in " << _binNumber << " bins !" << endl;
}

