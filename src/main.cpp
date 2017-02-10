#include <iostream>

#include <boost/program_options.hpp>

#include "Log.hpp"
#include "Outer.hpp"
#include "Parser.hpp"
#include "solver/box/Scanline.hpp"
#include "solver/box/TheSkyIsTheLimit.hpp"
#include "solver/box/ToInfinityAndBeyond.hpp"
#include "Merger.hpp"
#include "SimpleTransformer.hpp"
#include "Display.hpp"

namespace po = boost::program_options;
using namespace std;

int main(int argc, char** argv) {
    LOG(info) << "SUPER PACKER 3000\n===================" << endl;
    srand(time(0));
    //Parsing command line
    po::options_description
    desc("Packer usage : ./packer [options] input-file\nAllowed options ");
    desc.add_options()
    ("help,h", "produce help message")
    ("input-file,f", po::value<string>()->required(), "input file path")
    ("dup,d", po::value<bool>()->default_value(false),
     "choose if the packed shapes are duplicated (at the bottom of the page) or if we are overwriting the file")
    ("width,W", po::value<int>()->default_value(0), "width of the packing space (px)")
    ("height,H", po::value<int>()->default_value(0), "height of the packing space (px)")
    ("id", po::value<vector<string>>(), "ID of a specific element to be packed")
    ("nbMerge", po::value<int>()->default_value(2), "Number of merge steps")
    ("debug", po::value<bool>()->default_value(false),
     "Produce debug SVG instead of real one")
    ("buffer", po::value<double>()->default_value(0.),
     "minimal distance between packed items (px)");
    po::variables_map vm; //Parameters container
    po::positional_options_description p; //Used to indicate input file without --input-file
    p.add("input-file", -1);

    try {
        //Effectively parse the command line
        po::store(po::command_line_parser(argc, argv).
                  options(desc).positional(p).allow_unregistered().run(), vm);

        if (vm.count("help")) {
            cerr << desc << endl;
            return EXIT_SUCCESS;
        }

        //Check parsing errors (required parameters, ...)
        po::notify(vm);
    }
    catch (exception& e) {
        LOG(fatal) << "Error : " << e.what() << endl;
        return EXIT_FAILURE;
    }

    //Will contain the IDs the solver will pack
    vector<string> toPack;

    //If the user selected some shapes, add them to toPack
    if (vm.count("id"))
        toPack = vm["id"].as<vector<string>>();

    //Parsing input file, sending to the parser the ids of the shapes we want to keep
    vector<Shape> shapes = Parser::Parse(vm["input-file"].as<string>(), toPack);
    LOG(debug) << "Doc dimensions " << Parser::getDims().x() << " ; " << Parser::getDims().y()
               << endl;
    Display::Init(shapes);

    //If nothing was selected, fill toPack with every parsed ID
    if (!vm.count("id"))
        LOG(info) << "Will pack *all* shapes." << endl;

    for (auto && s : shapes)
        toPack.push_back(s.getIdentifier());

    //Apply a buffer to the shapes
    LOG(info) << "Buffering shapes..." << endl;

    for (auto && s : shapes)
        s.bufferize(vm["buffer"].as<double>());

    //If the user did not specify width, take document width for packing (idem for height)
    Parser::setDims(Point(
                        (vm["width"].as<int>() == 0) ? Parser::getDims().x() : vm["width"].as<int>(),
                        (vm["height"].as<int>() == 0) ? Parser::getDims().y() : vm["height"].as<int>()));
    //Prepacking the shapes
    Merger merger(shapes);
    SimpleTransformer trans(shapes);

    for (int i = 0 ; i < vm["nbMerge"].as<int>() ; ++i)
        merger.merge(trans.transform());

    Scanline solver(shapes);
    solver.solve();
    merger.reset();
    //Evaluating the quality
    LOG(info) << "Compression rate achieved : " << solver.compressionRatio() << endl;

    //Producing the output (sending input file and the option to duplicate
    if (vm["debug"].as<bool>())
        cout << solver.debugOutputSVG();
    else
        Outer::Write(vm["input-file"].as<string>(), vm["dup"].as<bool>(), toPack, shapes);

    return EXIT_SUCCESS;
}
