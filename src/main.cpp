#include <boost/program_options.hpp>
#include <iostream>
#include <ctime>

#include "Parser.hpp"
#include "IdentitySolver.hpp"
#include "ToInfinityAndBeyondSolver.hpp"

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
    ("width",  po::value<int>(), "width of the packing space (px)")
    ("height", po::value<int>(), "height of the packing space (px)")
    ("addto", po::value<bool>(), "adds the packed shapes at the bottom of the original file")
    ("id", po::value<vector<string>>(), "ID of a specific element to be packed");

    po::variables_map vm;
    po::positional_options_description p;
    p.add("input-file", -1);
    po::store(po::command_line_parser(argc, argv).
              options(desc).positional(p).allow_unregistered().run(), vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return EXIT_SUCCESS;
    }

    try {
        po::notify(vm);
    }
    catch (exception& e) {
        cout << "Error : " << e.what() << endl;
        return EXIT_FAILURE;
    }

    vector<string> toPack;

    if (vm.count("id")) {
        toPack = vm["id"].as<vector<string>>();
    }

    //Parsing input file, sengind the parser the ids of the shapes we want to keep
    vector<Shape> shapes = Parser::Parse(vm["input-file"].as<string>(),
                                         toPack);

    if (!vm.count("id"))
        for (auto && s : shapes) {
            toPack.push_back(s.getID());
        }

    //Packing the shapes
    //We should send width (vm["width"].as<int>()) and height (vm["height"].as<int>()) to
    //the solver if they are present
    ToInfinityAndBeyondSolver solver(shapes);
    solver.solve();

    //Producing the output
    //cout << solver.debugOutputSVG() << endl;
    cout << solver.outputSVG(vm["input-file"].as<string>(), vm.count("addto"),
                             toPack) << endl;
    return EXIT_SUCCESS;
}
