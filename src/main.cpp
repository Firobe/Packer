#include <iostream>

#include "Parser.h"
#include "IdentitySolver.h"

using namespace std;

int main(int argc, char** argv) {
	if(argc <= 1){
		cout << "Usage : ./parser input.svg > output.svg" << endl;
		return EXIT_FAILURE;
	}
    vector<Shape> shapes = Parser::Parse(argv[1]);

    IdentitySolver solver(shapes);
    solver.solve();

    cout << solver.debugOutputSVG() << endl;
    return EXIT_SUCCESS;
}
