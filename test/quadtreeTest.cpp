#include <iostream>
#include <vector>
#include <string>

#include <boost/geometry/algorithms/num_points.hpp>

#include "commonTest.hpp"
#include "Parser.hpp"
#include "Shape.hpp"
#include "solver/quadtree/quadTree.hpp"

using namespace std;

int main() {

    //Will contain the IDs the solver will pack
    vector<string> toPack;

    Point docDim; //Container for the document dimensions
    //Parsing input file, sending to the parser the ids of the shapes we want to keep
	vector<Shape> shapes = Parser::Parse("vertebrae.svg",
                                         toPack, docDim);

    for (Shape& s: shapes) {
        cout << bg::num_points(s.getMultiP()) << endl;
    }

	quadTree tree(shapes[0], 1000);

	return EXIT_SUCCESS;
}
