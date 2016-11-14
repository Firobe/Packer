#include <iostream>

#include "Parser.h"
#include "IdentitySolver.h"

using namespace std;

int main(int argc, char** argv) {
    //Sample use case

    vector<Shape> shapes = Parser::Parse(argv[1]);

    IdentitySolver solver(shapes);
    solver.solve();

    cout << solver.debugOutputSVG() << endl;
    return EXIT_SUCCESS;
}
