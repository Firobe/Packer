#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <typeinfo>
#include <chrono>
#include <stdexcept>

#include <boost/geometry.hpp>
// Plante sans cette include pour une raison indeterminé

#include <boost/geometry/algorithms/num_points.hpp>

#include "commonTest.hpp"
#include "Parser.hpp"
#include "Layout.hpp"
#include "quadtree/QuadTree.hpp"
#include "quadtree/bitmap.hpp"
#include <boost/geometry/algorithms/convert.hpp>

#define REPEAT 100000 //10^5
#define REPEAT2 1000 //10^4

using Clock = std::chrono::system_clock;
using namespace std;

int main() {

	srand(time(0));
	volatile bool res = 0;

	vector<string> toPack;
	Layout vertebraeQuad(Parser::Parse("vertebrae.svg", toPack), 5);
	Layout vertebraePoly(Parser::Parse("vertebrae.svg", toPack));

	//intersection time (default position)
	int repeat_nb = REPEAT2 * vertebraeQuad.size() * vertebraeQuad.size();
	auto cstart = Clock::now();
	for (unsigned i=0; i<REPEAT2;i++) {
		for (unsigned j=0; j<vertebraeQuad.size(); j++) {
			for (unsigned k=0; k<vertebraeQuad.size(); k++) {
				res = vertebraeQuad[j].intersectsWith(vertebraeQuad[k]);
			}
		}
	}
	auto cend = Clock::now();
	int celapsed1 = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cout << "QuadTree intersection time (default position) : " << celapsed1 << " us for " << repeat_nb << " execution" << endl <<
			"                                                " << celapsed1/repeat_nb << " us for a single execution" << endl;
	cstart = Clock::now();
	for (unsigned i=0; i<REPEAT2;i++) {
		for (unsigned j=0; j<vertebraePoly.size(); j++) {
			for (unsigned k=0; k<vertebraePoly.size(); k++) {
				res = vertebraePoly[j].intersectsWith(vertebraePoly[k]);
			}
		}
	}
	cend = Clock::now();
	int celapsed2 = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cout << "Polygon intersection time (default position) : " << celapsed2 << " us for " << repeat_nb << " execution" << endl <<
			"                                               " << celapsed2/repeat_nb << " us for a single execution" << endl;
	cout << "Ratio : " << celapsed2/celapsed1 << endl;

	//intersection time (origin position)
	Box b;
	for (unsigned i=0; i<vertebraePoly.size(); i++) {
		vertebraePoly[i].envelope(b);
		vertebraePoly[i].translate(-b.min_corner().x(), -b.min_corner().y());
		vertebraeQuad[i].translate(-b.min_corner().x(), -b.min_corner().y());
	}
	cstart = Clock::now();
	for (unsigned i=0; i<REPEAT2;i++) {
		for (unsigned j=0; j<vertebraeQuad.size(); j++) {
			for (unsigned k=0; k<vertebraeQuad.size(); k++) {
				res = vertebraeQuad[j].intersectsWith(vertebraeQuad[k]);
			}
		}
	}
	cend = Clock::now();
	celapsed1 = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cout << "QuadTree intersection time (origin position) : " << celapsed1 << " us for " << repeat_nb << " execution" << endl <<
			"                                               " << celapsed1/repeat_nb << " us for a single execution" << endl;
	cstart = Clock::now();
	for (unsigned i=0; i<REPEAT2;i++) {
		for (unsigned j=0; j<vertebraePoly.size(); j++) {
			for (unsigned k=0; k<vertebraePoly.size(); k++) {
				res = vertebraePoly[j].intersectsWith(vertebraePoly[k]);
			}
		}
	}
	cend = Clock::now();
	celapsed2 = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cout << "Polygon intersection time (origin position) : " << celapsed2 << " us for " << repeat_nb << " execution" << endl <<
			"                                              " << celapsed2/repeat_nb << " us for a single execution" << endl;
	cout << "Ratio : " << celapsed2/celapsed1 << endl;

	//Rotation time
	repeat_nb = REPEAT * vertebraePoly.size();
	cstart = Clock::now();
	for (unsigned i=0; i<REPEAT;i++) {
		for (unsigned k=0; k<vertebraeQuad.size(); k++) {
			vertebraeQuad[k].rotate(120);
		}
	}
	cend = Clock::now();
	celapsed1 = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cout << "QuadTree rotation time : " << celapsed1 << " us for " << repeat_nb << " execution" << endl <<
			"                         " << celapsed1/repeat_nb << " us for a single execution" << endl;
	cstart = Clock::now();
	for (unsigned i=0; i<REPEAT;i++) {
		for (unsigned k=0; k<vertebraePoly.size(); k++) {
			vertebraePoly[k].rotate(120);
		}
	}
	cend = Clock::now();
	celapsed2 = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cout << "Polygon rotation time : " << celapsed2 << " us for " << repeat_nb << " execution" << endl <<
			"                        " << celapsed2/repeat_nb << " us for a single execution" << endl;
	cout << "Ratio : " << celapsed2/celapsed1 << endl;


	//Translation time
	cstart = Clock::now();
	for (unsigned i=0; i<REPEAT;i++) {
		for (unsigned k=0; k<vertebraeQuad.size(); k++) {
			vertebraeQuad[k].translate(1,-1);
		}
	}
	cend = Clock::now();
	celapsed1 = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cout << "QuadTree translation time : " << celapsed1 << " us for " << repeat_nb << " execution" << endl <<
			"                         " << celapsed1/repeat_nb << " us for a single execution" << endl;
	cstart = Clock::now();
	for (unsigned i=0; i<REPEAT;i++) {
		for (unsigned k=0; k<vertebraePoly.size(); k++) {
			vertebraePoly[k].translate(1,-1);
		}
	}
	cend = Clock::now();
	celapsed2 = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cout << "Polygon translation time : " << celapsed2 << " us for " << repeat_nb << " execution" << endl <<
			"                        " << celapsed2/repeat_nb << " us for a single execution" << endl;
	cout << "Ratio : " << celapsed2/celapsed1 << endl;
	(void) res;
}
