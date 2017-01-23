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
#include "solver/quadtree/quadTree.hpp"
#include "solver/quadtree/bitmap.hpp"

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

    for (Shape& s: shapes) {
        cout << bg::num_points(s.getMultiP()) << endl;
		ofstream file;
		file.open(s.getID() + ".svg");
		file << s.debugOutputSVG();
		file.close();
    }

	vector<quadTree> quads;
	for (Shape& s: shapes) {
		quads.push_back(quadTree(s, 10));
		//bitmap(s, 64, 64);
	}

	cout << "Intersects : " << quads[1].intersects(quads[2]);
	cout << "-" << bg::intersects(shapes[1].getMultiP(), shapes[2].getMultiP()) << endl;
	//quadTree(shapes[0], 1);

	auto start = Clock::now();
	for (int i=0; i<100; i++)
		for (Shape& s1: shapes) {
			for (Shape& s2 : shapes)
				bg::intersects(s1.getMultiP(), s2.getMultiP());
		}
	auto end = Clock::now();
	int elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << elapsed << " microseconds elapsed" << endl;

	start = Clock::now();
	for (int i=0; i<100; i++)
		for (quadTree& s1: quads) {
			for (quadTree& s2 : quads)
				s1.intersects(s2);
		}
	end = Clock::now();
	elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << elapsed << " microseconds elapsed" << endl;

	start = Clock::now();
	for (int i=0; i<100*shapes.size(); i++)
		for (Shape& s1: shapes)
				translate<Shape>(s1,rand()*10-5, rand()*10-5);
	end = Clock::now();
	elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << elapsed << " microseconds elapsed" << endl;

	start = Clock::now();
	for (int i=0; i<100*quads.size(); i++)
		for (quadTree& s1: quads)
				s1.translater(rand()*10-5, rand()*10-5);
	end = Clock::now();
	elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << elapsed << " microseconds elapsed" << endl;


	/*for (int i = 0; i<quads.size(); i++) {
		for (int j = 0; j<quads.size(); j++) {
			cout << "Intersects : " << quads[i].intersects(quads[j]) << "-" << bg::intersects(shapes[i].getMultiP(), shapes[j].getMultiP()) << endl;
		}
	}*/

	return EXIT_SUCCESS;
}
