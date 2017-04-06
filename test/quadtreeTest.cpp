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

	/*// getTransformMatrix test
	cerr << "Testing getTransMatrix accuracy" << endl;
	array<double, 6> transformMatrix;

	shapes[0].rotate(90);
	transformMatrix = shapes[0].getTransMatrix();
	ASSERT(abs(transformMatrix[0] - 0)  < pow(10,-4), "transformMatrix 1-1");
	ASSERT(abs(transformMatrix[1] - 1)  < pow(10,-4), "transformMatrix 1-2");
	ASSERT(abs(transformMatrix[2] - -1) < pow(10,-4), "transformMatrix 1-3");
	ASSERT(abs(transformMatrix[3] - 0)  < pow(10,-4), "transformMatrix 1-4");
	ASSERT(abs(transformMatrix[4] - 0)  < pow(10,-4), "transformMatrix 1-5");
	ASSERT(abs(transformMatrix[5] - 0)  < pow(10,-4), "transformMatrix 1-6");

	shapes[0].translate(-100,-100);
	transformMatrix = shapes[0].getTransMatrix();
	ASSERT(abs(transformMatrix[0] - 0)    < pow(10,-4), "transformMatrix 2-1");
	ASSERT(abs(transformMatrix[1] - 1)    < pow(10,-4), "transformMatrix 2-2");
	ASSERT(abs(transformMatrix[2] - -1)   < pow(10,-4), "transformMatrix 2-3");
	ASSERT(abs(transformMatrix[3] - 0)    < pow(10,-4), "transformMatrix 2-4");
	ASSERT(abs(transformMatrix[4] - -100) < pow(10,-4), "transformMatrix 2-5");
	ASSERT(abs(transformMatrix[5] - -100) < pow(10,-4), "transformMatrix 2-6");

	shapes[3].rotate(90);
	shapes[3].translate(0,50);
	transformMatrix = shapes[3].getTransMatrix();
	ASSERT(abs(transformMatrix[0] - 0)    < pow(10,-4), "transformMatrix 3-1");
	ASSERT(abs(transformMatrix[1] - 1)    < pow(10,-4), "transformMatrix 3-2");
	ASSERT(abs(transformMatrix[2] - -1)   < pow(10,-4), "transformMatrix 3-3");
	ASSERT(abs(transformMatrix[3] - 0)    < pow(10,-4), "transformMatrix 3-4");
	ASSERT(abs(transformMatrix[4] - -150) < pow(10,-4), "transformMatrix 3-5");
	ASSERT(abs(transformMatrix[5] - 100)  < pow(10,-4), "transformMatrix 3-6");

	cerr << "No error found" << endl;*/

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




	Layout vertebraeQuad(Parser::Parse("vertebrae.svg", toPack), 5);
	Layout vertebraePoly(Parser::Parse("vertebrae.svg", toPack));

	// MergeWith tests















	cstart = Clock::now();
	for (int i=0; i<REPEAT; i++)
		for (Shape& s1: vertebraePoly) {
			for (Shape& s2 : vertebraePoly)
				s1.intersectsWith(s2);
		}
	cend = Clock::now();
	celapsed = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cerr << "Sh int : " << celapsed << " microseconds elapsed" << endl;

	cstart = Clock::now();
	for (int i=0; i<REPEAT; i++)
		for (Shape& s1: vertebraeQuad) {
			for (Shape& s2 : vertebraeQuad)
				s1.intersectsWith(s2);
		}
	cend = Clock::now();
	int celapsed2 = chrono::duration_cast<chrono::microseconds>(cend - cstart).count();
	cerr << "qd int : " << celapsed2 << " microseconds elapsed" << endl;
	cerr << "  - ratio : " << (double) celapsed/celapsed2 << endl;

	/*bitmap bmap(shapes[1].getMultiP(), 99, 100);
	bmap.saveMap("test");
	for (size_t i=0; i<quads.size(); i++)
		quads[i].saveTree(shapes[i].getID());*/

	/*auto start = Clock::now();
	for (int i=0; i<REPEAT; i++)
		for (Shape& s1: shapes) {
			for (Shape& s2 : shapes)
				bg::intersects(s1.getMultiP(), s2.getMultiP());
		}
	auto end = Clock::now();
	int elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "Sh int : " << elapsed << " microseconds elapsed" << endl;

	//for (QuadTree& s1: quads)
	//		s1.translater(0.5, 0.5);

	start = Clock::now();
	for (int i=0; i<REPEAT; i++) {
		for (QuadTree& s1 : quads) {
			for (QuadTree& s2 : quads)
				s1.intersects(s2);
		}
	}

	end = Clock::now();
	int elapsed2 = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "qd int : " << elapsed2 << " microseconds elapsed" << endl;
	cout << "  - ratio : " << (double) elapsed/elapsed2 << endl;*/

	/*start = Clock::now();
	for (vector<Shape>::size_type i=0; i<REPEAT*shapes.size(); i++)
		for (Shape& s1: shapes)
				translate<Shape>(s1,rand()*10-5, rand()*10-5);
	end = Clock::now();
	elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "sh tra : " << elapsed << " microseconds elapsed" << endl;

	start = Clock::now();
	for (vector<QuadTree>::size_type i=0; i<100*quads.size(); i++)
		for (QuadTree& s1: quads)
				s1.translater(rand()*10-5, rand()*10-5);
	end = Clock::now();
	elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "qd tra : " << elapsed << " microseconds elapsed" << endl;*/

	/*start = Clock::now();
	for (vector<QuadTree>::size_type i=0; i<REPEAT*quads.size(); i++)
		for (QuadTree& s1: quads)
				s1.deepTranslater(rand()*10-5, rand()*10-5);
	end = Clock::now();
	elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "qd tra : " << elapsed << " microseconds elapsed" << endl;*/

	/*start = Clock::now();
	for (vector<Shape>::size_type i=0; i<REPEAT*shapes.size(); i++)
		for (Shape& s1: shapes)
				rotate<Shape>(s1,6);
	end = Clock::now();
	elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "sh rot : " << elapsed << " microseconds elapsed" << endl;

	start = Clock::now();
	for (int i=0; i<REPEAT; i++)
		for (Shape& s1: shapes)
				QuadTree(s1, 10);
	end = Clock::now();
	elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "Qd cre : " << elapsed << " microseconds elapsed" << endl;*/

	/*vector<bitmap> bits;
	bits.reserve(shapes.size());
	for (Shape& s: shapes) {
		bits.emplace_back(s, 4);
		bits.back().saveMap("0" + s.getID());
	}

	// Intersection accuracy verification
	diff = 0;
	for(size_t i=0; i<bits.size(); i++) {
		for(size_t j=i+1; j<bits.size(); j++) {
			bool b1 = bits[i].intersects(bits[j]);
			bool b2 = bg::intersects(shapes[i].getMultiP(), shapes[j].getMultiP());
			//cout << "Shape : " << shapes[i].getID() << "-" << shapes[j].getID() << endl;
			//cout << "Inter : " << b1 << "-" << b2 << endl;
			if (b1 && !b2) diff++;
			if (!b1 && b2) throw "Bitmap accuracy critical error";
		}
	}
	cout << "bitmap approximations errors : " << diff << endl;

	start = Clock::now();
	for (int i=0; i<REPEAT; i++) {
		for (bitmap& s1 : bits) {
			for (bitmap& s2 : bits)
				s1.intersects(s2);
		}
	}
	end = Clock::now();
	elapsed2 = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "bm int : " << elapsed2 << " microseconds elapsed" << endl;
	cout << "  - ratio : " << (double) elapsed/elapsed2 << endl;

	start = Clock::now();
	int tx, ty;
	//for (int j=0; j<REPEAT; j++)
		for (size_t i=0; i<bits.size(); i++) {
			bitmap::trim(bitmap::rotate(&bits[i], 1, tx, ty), tx, ty)->saveMap("0" + shapes[i].getID() + "rotated");
		}
	end = Clock::now();
	elapsed2 = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "bm rot : " << elapsed2 << " microseconds elapsed" << endl;*/


	/*ofstream file;
	file.open("rotater.svg");
	int pos = 0;
	for (QuadTree &quad : quads) {
		quad.rotater(45);
		quad.applyRotation();
		quad.saveTree("rot" + shapes[pos].getID());

		rotate<Shape>(shapes[pos],45);

		pos++;
	}

	// Intersection accuracy verification
	diff = 0;
	crit = 0;
	for(size_t i=0; i<quads.size(); i++) {
		for(size_t j=i+1; j<quads.size(); j++) {
			bool b1 = quads[i].intersects(quads[j]);
			bool b2 = bg::intersects(shapes[i].getMultiP(), shapes[j].getMultiP());
			//cout << "Shape : " << shapes[i].getID() << "-" << shapes[j].getID() << endl;
			//cout << "Inter : " << b1 << "-" << b2 << endl;
			if (b1 && !b2) diff++;
			if (!b1 && b2) crit++;
		}
	}
	cout << "QuadTrees approximations errors : " << diff << endl;
	cout << "QuadTree accuracy critical error : " << crit << endl;*/

	//quads[1].saveTree("test-init");
	/*cout << endl << quads[1] << endl;

	for (int i=0; i<1; i++) {
		quads[1].rotater(100);
		cout << quads[1] << endl;
	}*/

	//quads[1].saveTree("test-rota");

	return EXIT_SUCCESS;
}
