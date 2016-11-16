#include <boost/geometry/io/svg/svg_mapper.hpp>
#include <sstream>
#include <string>

#include "Solver.hpp"

using namespace std;

/**
 * Returns a string corresponding to the SVG file to ouput.
 * (Theoretically with the packed shapes)
 */
string Solver::outputSVG() {
    ///TODO
    return "";
}

string Solver::debugOutputSVG() {
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
