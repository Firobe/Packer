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

int main() {
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


	for(size_t i=0; i<shapes.size(); i++) {
		QuadTree& quad = dynamic_cast<QuadTree&>(shapes[i]);
		cout << quad << endl;
	}

	// Intersection accuracy verification
	cerr << "Testting intersection accuracy :" << endl;
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
