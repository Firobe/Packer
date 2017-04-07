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

	vector<string> toPack;
	Layout vertebraeQuad(Parser::Parse("vertebrae.svg", toPack), 5);
	Layout vertebraeQuad2(Parser::Parse("vertebrae.svg", toPack), 5);
	Layout vertebraePoly(Parser::Parse("vertebrae.svg", toPack));
	Layout vertebraePoly2(Parser::Parse("vertebrae.svg", toPack));

	//intersection time (default position)
	int repeat_nb = REPEAT2 * vertebraeQuad.size() * vertebraeQuad.size();
	auto cstart = Clock::now();
	for (unsigned i=0; i<REPEAT2;i++) {
		for (unsigned j=0; j<vertebraeQuad.size(); j++) {
			for (unsigned k=0; k<vertebraeQuad.size(); k++) {
				vertebraeQuad[j].intersectsWith(vertebraeQuad[k]);
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
				vertebraePoly[j].intersectsWith(vertebraePoly[k]);
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
				vertebraeQuad[j].intersectsWith(vertebraeQuad[k]);
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
				vertebraePoly[j].intersectsWith(vertebraePoly[k]);
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
	int transtime = celapsed2;
	cout << "Polygon translation time : " << celapsed2 << " us for " << repeat_nb << " execution" << endl <<
			"                        " << celapsed2/repeat_nb << " us for a single execution" << endl;
	cout << "Ratio : " << celapsed2/celapsed1 << endl;


	//centroid time
	cstart = Clock::now();
	for (unsigned i=0; i<REPEAT;i++) {
		for (unsigned k=0; k<vertebraeQuad.size(); k++) {
			vertebraeQuad[k].centroid();
		}
	}
	cend = Clock::now();
	celapsed1 = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cout << "QuadTree centroid time : " << celapsed1 << " us for " << repeat_nb << " execution" << endl <<
			"                         " << celapsed1/repeat_nb << " us for a single execution" << endl;
	cstart = Clock::now();
	for (unsigned i=0; i<REPEAT;i++) {
		for (unsigned k=0; k<vertebraePoly.size(); k++) {
			vertebraePoly[k].centroid();
			vertebraePoly[k].translate(1,-1); // in order to prevent Boost optimization, time is deduced after
		}
	}
	cend = Clock::now();
	celapsed2 = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cout << "Polygon centroid time : " << celapsed2-transtime << " us for " << repeat_nb << " execution" << endl <<
			"                        " << (celapsed2-transtime)/repeat_nb << " us for a single execution" << endl;
	cout << "Ratio : " << celapsed2/celapsed1 << endl;


	//envelope time
	cstart = Clock::now();
	for (unsigned i=0; i<REPEAT;i++) {
		for (unsigned k=0; k<vertebraeQuad.size(); k++) {
			vertebraeQuad[k].envelope(b);
		}
	}
	cend = Clock::now();
	celapsed1 = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cout << "QuadTree envelope time : " << celapsed1 << " us for " << repeat_nb << " execution" << endl <<
			"                         " << celapsed1/repeat_nb << " us for a single execution" << endl;
	cstart = Clock::now();
	for (unsigned i=0; i<REPEAT;i++) {
		for (unsigned k=0; k<vertebraePoly.size(); k++) {
			vertebraePoly[k].envelope(b);
			vertebraePoly[k].translate(1,-1); // in order to prevent Boost optimization, time is deduced after
		}
	}
	cend = Clock::now();
	celapsed2 = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cout << "Polygon envelope time : " << celapsed2-transtime << " us for " << repeat_nb << " execution" << endl <<
			"                        " << (celapsed2-transtime)/repeat_nb << " us for a single execution" << endl;
	cout << "Ratio : " << celapsed2/celapsed1 << endl;

	//area time
	cstart = Clock::now();
	for (unsigned i=0; i<REPEAT;i++) {
		for (unsigned k=0; k<vertebraeQuad.size(); k++) {
			vertebraeQuad[k].area();
		}
	}
	cend = Clock::now();
	celapsed1 = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cout << "QuadTree area time : " << celapsed1 << " us for " << repeat_nb << " execution" << endl <<
			"                         " << celapsed1/repeat_nb << " us for a single execution" << endl;
	cstart = Clock::now();
	for (unsigned i=0; i<REPEAT;i++) {
		for (unsigned k=0; k<vertebraePoly.size(); k++) {
			vertebraePoly[k].area();
			vertebraePoly[k].translate(1,-1); // in order to prevent Boost optimization, time is deduced after
		}
	}
	cend = Clock::now();
	celapsed2 = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cout << "Polygon area time : " << celapsed2-transtime << " us for " << repeat_nb << " execution" << endl <<
			"                        " << (celapsed2-transtime)/repeat_nb << " us for a single execution" << endl;
	cout << "Ratio : " << celapsed2/celapsed1 << endl;


	// merge WIth
	for (unsigned i=0; i<vertebraePoly.size(); i++) {
		vertebraePoly[i].envelope(b);
		vertebraePoly[i].translate(-b.min_corner().x(), -b.min_corner().y());
		vertebraeQuad[i].envelope(b);
		vertebraeQuad[i].translate(-b.min_corner().x(), -b.min_corner().y());
		vertebraePoly2[i].envelope(b);
		vertebraePoly2[i].translate(-b.min_corner().x(), -b.min_corner().y());
		vertebraePoly2[i].translate(50,50);
		vertebraeQuad2[i].envelope(b);
		vertebraeQuad2[i].translate(-b.min_corner().x(), -b.min_corner().y());
		vertebraeQuad2[i].translate(50, 50);
	}
	repeat_nb = vertebraePoly.size();
	cstart = Clock::now();
	for (unsigned k=1; k<vertebraeQuad.size(); k++) {
		vertebraeQuad[k].mergeWith(vertebraeQuad2[k]);
	}
	cend = Clock::now();
	celapsed1 = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cout << "QuadTree merge time : " << celapsed1 << " us for " << repeat_nb << " execution" << endl <<
			"                         " << celapsed1/repeat_nb << " us for a single execution" << endl;
	cstart = Clock::now();
	for (unsigned k=1; k<vertebraePoly.size(); k++) {
		vertebraePoly[2].mergeWith(vertebraePoly2[k]);

	}
	cend = Clock::now();
	celapsed2 = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cout << "Polygon merge time : " << celapsed2 << " us for " << repeat_nb << " execution" << endl <<
			"                        " << celapsed2/repeat_nb << " us for a single execution" << endl;
	cout << "Ratio : " << celapsed2/(float) celapsed1 << endl;
}
