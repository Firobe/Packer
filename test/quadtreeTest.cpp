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

#define REPEAT 100

using Clock = std::chrono::system_clock;
using namespace std;

static float epsilon = pow(10,-4);

static bool boxEqual(Box &ba, Box &bb) {
	return (abs(ba.min_corner().x() - bb.min_corner().x()) < epsilon) &&
			(abs(ba.min_corner().y() - bb.min_corner().y()) < epsilon) &&
			(abs(ba.max_corner().x() - bb.max_corner().x()) < epsilon) &&
			(abs(ba.max_corner().y() - bb.max_corner().y()) < epsilon);
}

static bool MatrixEqual(array<double,6> &ma, array<double,6> &mb) {
	return (abs(ma[0] - mb[0]) < epsilon) &&
			(abs(ma[1] - mb[1]) < epsilon) &&
			(abs(ma[2] - mb[2]) < epsilon) &&
			(abs(ma[3] - mb[3]) < epsilon) &&
			(abs(ma[4] - mb[4]) < epsilon) &&
			(abs(ma[5] - mb[5]) < epsilon);
}

static bool PointEqual(Point &pa, Point &pb) {
	return (abs(pa.x() - pb.x()) < epsilon) &&
			(abs(pa.y() - pb.y()) < epsilon);
}

int main() {
	static bool verboseDebug = false;

	srand(time(0));

	//Will contain the IDs the solver will pack
	vector<string> toPack;

	//Parsing input file, sending to the parser the ids of the shapes we want to keep
	auto cstart = Clock::now();

	Layout shapes(Parser::Parse("quadtreeTest.svg", toPack), 5);
	ASSERT(shapes.size() == 6, "wrong number of shapes");

	auto cend = Clock::now();
	int celapsed = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cerr << "Parsing and quadtree creation time : " << celapsed << " microseconds elapsed" << endl;

	Layout shapes2(Parser::Parse("quadtreeTest.svg", toPack));

	if (verboseDebug) {
		for(size_t i=0; i<shapes.size(); i++) {
			QuadTree& quad = dynamic_cast<QuadTree&>(shapes[i]);
			cout << quad << endl;
		}
	}

	// Intersection accuracy verification
	cerr << "Testing intersection accuracy" << endl;
	bool noerr=true;
	for(size_t i=0; i<shapes.size(); i++) {
		for(size_t j=i+1; j<shapes.size(); j++) {
			if (shapes[i].intersectsWith(shapes[j])) {
				cerr << " - Error between : " << i << " and " << j << endl;
				noerr = false;
			}
		}
	}
	if (noerr)
		cerr << "No error found" << endl;
	else
		cerr << "Some errors where find, see upside for more details" << endl;


	//Translation tests
	cerr << "Testing Translation accuracy" << endl;
	shapes[3].translate(-50, -150);
	shapes2[3].translate(-50, -150);
	ASSERT( shapes[0].intersectsWith(shapes[3]), "First Translation error 1");
	ASSERT( shapes[1].intersectsWith(shapes[3]), "First Translation error 2");
	ASSERT(!shapes[2].intersectsWith(shapes[3]), "First Translation error 3");
	ASSERT( shapes[4].intersectsWith(shapes[3]), "First Translation error 4");
	ASSERT( shapes[5].intersectsWith(shapes[3]), "First Translation error 5");

	shapes[2].translate(50,-150);
	shapes2[2].translate(50, -150);
	ASSERT(shapes[0].intersectsWith(shapes[2]), "Second Translation error 1");
	ASSERT(shapes[1].intersectsWith(shapes[2]), "Second Translation error 2");
	ASSERT(shapes[3].intersectsWith(shapes[2]), "Second Translation error 3");
	ASSERT(shapes[4].intersectsWith(shapes[2]), "Second Translation error 4");
	ASSERT(shapes[5].intersectsWith(shapes[2]), "Second Translation error 5");
	cerr << "No error found" << endl;

	// Rotation test
	cerr << "Testing Rotation accuracy" << endl;
	Box boxQ, boxC;
	array<double,6> matQ, matC;
	if (verboseDebug)
		cout << dynamic_cast<QuadTree&>(shapes[0]) << endl;
	for (int k=0; k<6; k++) {
		for (int i=0; i<12; i++) {
			shapes[k].rotate(30);
			shapes2[k].rotate(30);
			matQ = shapes[k].getTransMatrix();
			matC = shapes2[k].getTransMatrix();
			//cerr << matQ << " - " << matC << endl;
			ASSERT(MatrixEqual(matQ, matC), "ROTATION (mat) " + to_string(k) + "-" + to_string(i));
			shapes[k].envelope(boxQ);
			shapes2[k].envelope(boxC);
			//cerr << bg::wkt(boxQ.min_corner()) << "." << bg::wkt(boxQ.max_corner()) << " - "
			//	 << bg::wkt(boxC.min_corner()) << "." << bg::wkt(boxC.max_corner()) << endl;
			ASSERT(boxEqual(boxQ, boxC), "ROTATION (box) " + to_string(k) + "-" + to_string(i));
			if (verboseDebug)
				cout << dynamic_cast<QuadTree&>(shapes[k]) << endl;
		}
	}
	cerr << "No error found" << endl;

	// Rotations and translations
	cerr << "Testing Rotation with translation accuracy" << endl;
	if (verboseDebug)
		cout << dynamic_cast<QuadTree&>(shapes[0]) << endl;
	for (int k=0; k<6; k++) {
		for (int i=0; i<12; i++) {
			shapes[k].rotate(30);
			shapes2[k].rotate(30);
			shapes[k].translate(50,-50);
			shapes2[k].translate(50,-50);
			matQ = shapes[k].getTransMatrix();
			matC = shapes2[k].getTransMatrix();
			//cerr << matQ << " - " << matC << endl;
			ASSERT(MatrixEqual(matQ, matC), "ROTATION AND TRANSLATION (mat) " + to_string(k) + "-" + to_string(i));
			shapes[k].envelope(boxQ);
			shapes2[k].envelope(boxC);
			//cerr << bg::wkt(boxQ.min_corner()) << "." << bg::wkt(boxQ.max_corner()) << " - "
			//	 << bg::wkt(boxC.min_corner()) << "." << bg::wkt(boxC.max_corner()) << endl;
			ASSERT(boxEqual(boxQ, boxC), "ROTATION AND TRANSLATION (box) " + to_string(k) + "-" + to_string(i));
			if (verboseDebug)
				cout << dynamic_cast<QuadTree&>(shapes[k]) << endl;
		}
	}
	cerr << "No error found" << endl;

	// centroid test
	cerr << "Testing centroid accuracy" << endl;
	Point pQ, pC;
	for (int k=0; k<6; k++) {
		for (int i=0; i<12; i++) {
			shapes[k].rotate(30);
			shapes2[k].rotate(30);
			pQ = shapes[k].centroid();
			pC = shapes2[k].centroid();
			ASSERT(PointEqual(pQ, pC), "CENTROID (dot) " + to_string(k) + "-" + to_string(i));
			if (verboseDebug)
				cout << dynamic_cast<QuadTree&>(shapes[k]) << endl;

		}
	}
	cerr << "No error found" << endl;

	// mergeWith
	shapes[0].rotate(60);
	shapes[0].translate(50,125);
	shapes[2].rotate(-60);
	shapes[2].translate(80,94);
	shapes2[0].rotate(60);
	shapes2[0].translate(50,125);
	shapes2[2].rotate(-60);
	shapes2[2].translate(80,94);
	shapes[0].mergeWith(shapes[2]);
	shapes2[0].mergeWith(shapes2[2]);
	matQ = shapes[0].getTransMatrix();
	matC = shapes2[0].getTransMatrix();
	ASSERT(MatrixEqual(matQ, matC), "MERGE WITH (mat)");
	pQ = shapes[0].centroid();
	pC = shapes2[0].centroid();
	ASSERT(PointEqual(pQ, pC), "MERGE WITH (dot)");
	shapes[0].envelope(boxQ);
	shapes2[0].envelope(boxC);
	ASSERT(boxEqual(boxQ, boxC), "MERGE WITH (box)");

	return EXIT_SUCCESS;
}
