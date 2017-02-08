#include <string>
#include <stdexcept>

#include "Parser.hpp"
#include "Shape.hpp"
#include "commonTest.hpp"

//sugina
using namespace std;

int main() {
    try {
        vector<string> toPack;
        vector<Shape> shapes = Parser::Parse(TEST_DIR + "test.svg", toPack);

        //Testing base
        ASSERT(Parser::getDims().x() == 500, "Bad doc width");
        ASSERT(Parser::getDims().y() == 1000, "Bad doc height");
        ASSERT(shapes.size() == 3, "Invalid number of shapes");

        //Testing IDs
        ASSERT(shapes[0].getIdentifier() == "path333600", "Bad ID 1");
        ASSERT(shapes[1].getIdentifier() == "path335700", "Bad ID 2");
        ASSERT(shapes[2].getIdentifier() == "g346500", "Bad ID 3");

        //Testing translation of the star
        ASSERT(abs(shapes[1].getMultiP()[0].outer()[0].x() - 212.18985) < EPSILON, "Bad X translation");
        ASSERT(abs(shapes[1].getMultiP()[0].outer()[0].y() - 948.67991) < EPSILON, "Bad Y translation");

		//Testing internal group translation
		ASSERT(abs(abs(shapes[2].getMultiP()[0].outer()[0].x() - shapes[2].getMultiP()[1].outer()[0].x())
			   	- 19.19287) < EPSILON, "Internal group translation");
		ASSERT(abs(abs(shapes[2].getMultiP()[0].outer()[0].y() - shapes[2].getMultiP()[1].outer()[0].y())
			   	- 493.9646) < EPSILON, "Internal group translation 2");
        }
    catch (exception& e) {
        cerr << "Failed : " << e.what() << endl;
		return EXIT_FAILURE;
        }

    return EXIT_SUCCESS;
    }
