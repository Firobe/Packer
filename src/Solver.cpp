#include "Solver.h"

#include <sstream>

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
    bg::svg_mapper<Point> mapper(ret, 400, 400);

    for (Shape& s : _shapes) {
        mapper.add(s.getRing());
    }

    for (Shape& s : _shapes) {
        mapper.map(s.getRing(), "");
    }

    return ret.str() + "</svg>";
}
