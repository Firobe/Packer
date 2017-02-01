#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <typeinfo>
#include <chrono>

#include <boost/geometry.hpp>
// Plante sans cette include pour une raison indeterminé

#include <boost/geometry/algorithms/num_points.hpp>

#include "commonTest.hpp"
#include "Parser.hpp"
#include "Shape.hpp"
#include "solver/quadtree/QuadTree.hpp"
#include "solver/quadtree/bitmap.hpp"
#include <boost/geometry/algorithms/convert.hpp>

#define REPEAT 100

using Clock = std::chrono::system_clock;
using namespace std;

int main() {
	srand(time(0));

    //Will contain the IDs the solver will pack
    vector<string> toPack;

    Point docDim; //Container for the document dimensions
    //Parsing input file, sending to the parser the ids of the shapes we want to keep
	vector<Shape> shapes = Parser::Parse("vertebrae.svg",
                                         toPack, docDim);

	/*for (Shape& s: shapes) {
        cout << bg::num_points(s.getMultiP()) << endl;
		ofstream file;
		file.open(s.getID() + ".svg");
		file << s.debugOutputSVG();
		file.close();
	}*/

	vector<QuadTree> quads;
	for (Shape& s: shapes) {
		quads.push_back(QuadTree(s, 5));
		cout << " - QuadTree : " << endl << quads.back() << endl;
		//bitmap(s, 64, 64);
	}

	// Intersection accuracy verification
	int diff = 0;
	for(size_t i=0; i<quads.size(); i++) {
		for(size_t j=i+1; j<quads.size(); j++) {
			bool b1 = quads[i].intersects(quads[j]);
			bool b2 = bg::intersects(shapes[i].getMultiP(), shapes[j].getMultiP());
			cout << "Shape : " << shapes[i].getID() << "-" << shapes[j].getID() << endl;
			cout << "Inter : " << b1 << "-" << b2 << endl;
			if (b1 && !b2) diff++;
			if (!b1 && b2) throw "QuadTree accuracy critical error";
		}
	}
	cout << "QuadTrees approximations errors : " << diff << endl;

	bitmap bmap(shapes[1].getMultiP(), 99, 100);
	bmap.saveMap("test");
	for (size_t i=0; i<quads.size(); i++)
		quads[i].saveTree(shapes[i].getID());

	auto start = Clock::now();
	for (int i=0; i<REPEAT; i++)
		for (Shape& s1: shapes) {
			for (Shape& s2 : shapes)
				bg::intersects(s1.getMultiP(), s2.getMultiP());
		}
	auto end = Clock::now();
	int elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "Sh int : " << elapsed << " microseconds elapsed" << endl;

	for (QuadTree& s1: quads)
			s1.translater(0.5, 0.5);

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
	cout << "  - ratio : " << (double) elapsed/elapsed2 << endl;

	start = Clock::now();
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
	cout << "qd tra : " << elapsed << " microseconds elapsed" << endl;

	/*start = Clock::now();
	for (vector<QuadTree>::size_type i=0; i<REPEAT*quads.size(); i++)
		for (QuadTree& s1: quads)
				s1.deepTranslater(rand()*10-5, rand()*10-5);
	end = Clock::now();
	elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "qd tra : " << elapsed << " microseconds elapsed" << endl;*/

	start = Clock::now();
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
	cout << "Qd cre : " << elapsed << " microseconds elapsed" << endl;

	return EXIT_SUCCESS;
}
