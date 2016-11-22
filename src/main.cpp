#include <boost/program_options.hpp>
#include <iostream>
#include <ctime>

#include "Outer.hpp"
#include "Parser.hpp"
#include "IdentitySolver.hpp"
#include "ToInfinityAndBeyondSolver.hpp"
#include "TheSkyIsTheLimitSolver.hpp"

namespace po = boost::program_options;
using namespace std;

int main(int argc, char** argv) {
    srand(time(0));
    //Parsing command line
    po::options_description
    desc("Packer usage : ./packer [options] input-file\nAllowed options ");
    desc.add_options()
    ("help", "produce help message")
    ("input-file", po::value<string>()->required(), "input file path")
    ("dup", po::value<bool>()->required(),
     "choose if the packed shapes are duplicated (at the bottom of the page) or if we are overwriting the file")
    ("width",  po::value<int>(), "width of the packing space (px)")
    ("height", po::value<int>(), "height of the packing space (px)")
    ("id", po::value<vector<string>>(), "ID of a specific element to be packed");

    po::variables_map vm;
    po::positional_options_description p;
    p.add("input-file", -1);

    try {
        po::store(po::command_line_parser(argc, argv).
                  options(desc).positional(p).allow_unregistered().run(), vm);

        if (vm.count("help")) {
            cerr << desc << endl;
            return EXIT_SUCCESS;
        }

        po::notify(vm);
    }
    catch (exception& e) {
        cerr << "Error : " << e.what() << endl;
        return EXIT_FAILURE;
    }

    vector<string> toPack;

    if (vm.count("id")) {
        toPack = vm["id"].as<vector<string>>();
    }

    double docHeight;
    //Parsing input file, sengind the parser the ids of the shapes we want to keep
    vector<Shape> shapes = Parser::Parse(vm["input-file"].as<string>(),
                                         toPack, docHeight);
    cerr << "Doc height " << docHeight << endl;

    if (!vm.count("id"))
        for (auto && s : shapes) {
            toPack.push_back(s.getID());
        }

    //Packing the shapes
    //We should send width (vm["width"].as<int>()) and height (vm["height"].as<int>()) to
    //the solver if they are present
    TheSkyIsTheLimitSolver solver(shapes,vm["width"].as<double>(),vm["height"].as<double>());
    solver.solve();

    //Producing the output
    cout << Outer::String(vm["input-file"].as<string>(), vm["dup"].as<bool>(), toPack,
                          docHeight, shapes);

    return EXIT_SUCCESS;
}
