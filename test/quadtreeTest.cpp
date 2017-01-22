#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <boost/geometry/algorithms/num_points.hpp>

#include "commonTest.hpp"
#include "Parser.hpp"
#include "Shape.hpp"
#include "solver/quadtree/quadTree.hpp"
#include "solver/quadtree/bitmap.hpp"

using namespace std;

int main() {

    //Will contain the IDs the solver will pack
    vector<string> toPack;

    Point docDim; //Container for the document dimensions
    //Parsing input file, sending to the parser the ids of the shapes we want to keep
	vector<Shape> shapes = Parser::Parse("test.svg",
                                         toPack, docDim);

    for (Shape& s: shapes) {
        cout << bg::num_points(s.getMultiP()) << endl;
		ofstream file;
		file.open(s.getID() + ".svg");
		file << s.debugOutputSVG();
		file.close();
    }

	for (Shape& s: shapes) {
		quadTree(s, 1);
		//bitmap(s, 64, 64);
	}

	//quadTree(shapes[0], 1);



	return EXIT_SUCCESS;
}
