#include <iostream>

#include "Parser.h"
#include "IdentitySolver.h"

using namespace std;

int main(int argc, char** argv) {
    //Sample use case

    vector<Shape> shapes = Parser::Parse("test.svg");

    IdentitySolver solver(shapes);
    solver.solve();

    cout << "Identity ouput : " << solver.outputSVG() << endl;
    return EXIT_SUCCESS;
}
