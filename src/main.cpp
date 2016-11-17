#include <boost/program_options.hpp>
#include <iostream>
#include <ctime>

#include "Parser.hpp"
#include "IdentitySolver.hpp"
#include "ToInfinityAndBeyondSolver.hpp"

namespace po = boost::program_options;
using namespace std;

int main(int argc, char** argv) {
	po::options_description desc("Packer usage");
	desc.add_options()
		("help", "produce help message");
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	if(vm.count("help")){
		cout << desc << endl;
		return EXIT_SUCCESS;
	}
    srand(time(0));

    vector<Shape> shapes = Parser::Parse(argv[1]);

    ToInfinityAndBeyondSolver solver(shapes);
    solver.solve();

    cout << solver.debugOutputSVG() << endl;
    return EXIT_SUCCESS;
}
