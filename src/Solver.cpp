#include "Solver.h"

#include <fstream>

using namespace std;

/**
 * Returns a string corresponding to the SVG file to ouput.
 * (Theoretically with the packed shapes)
 */
string Solver::outputSVG() {
    ///TODO
    return "";
}

string Solver::debugOutputSVG(){
	ofstream svg("outpute.svg");
	bg::svg_mapper<Point> mapper(svg, 400, 400);
	for(Shape& s : _shapes)
		mapper.add(s.getRing());
	for(Shape& s : _shapes)
		mapper.map(s.getRing(), "");
	return "";
}
