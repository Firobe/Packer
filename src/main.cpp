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

namespace po = boost::program_options;
using namespace std;

int main(int argc, char** argv) {
    LOG(info) << "SUPER PACKER 2000\n===================" << endl;
    srand(time(0));
    //Parsing command line
    po::options_description
    desc("Packer usage : ./packer [options] input-file\nAllowed options ");
    desc.add_options()
    ("help,H", "produce help message")
    ("input-file,I", po::value<string>()->required(), "input file path")
    ("dup", po::value<bool>()->required(),
     "choose if the packed shapes are duplicated (at the bottom of the page) or if we are overwriting the file")
    ("width,w", po::value<int>(), "width of the packing space (px)")
    ("height,h", po::value<int>(), "height of the packing space (px)")
    ("id", po::value<vector<string>>(), "ID of a specific element to be packed")
    ("buffer", po::value<int>(), "minimal distance between packed items (px)");
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
    if (vm.count("id")) {
        toPack = vm["id"].as<vector<string>>();
    }

    Point docDim; //Container for the document dimensions
    //Parsing input file, sending to the parser the ids of the shapes we want to keep
    vector<Shape> shapes = Parser::Parse(vm["input-file"].as<string>(),
                                         toPack, docDim);
    LOG(debug) << "Doc dimensions " << docDim.x() << " ; " << docDim.y() << endl;

    //If nothing was selected, fill toPack with every parsed ID
    if (!vm.count("id")) {
        LOG(info) << "Will pack *all* shapes." << endl;
    }

    for (auto && s : shapes) {
        toPack.push_back(s.getID());
    }

    //If there is a buffer distance specified
    if (vm.count("buffer") && vm["buffer"].as<int>() > 0) {
        LOG(info) << "Buffering shapes..." << endl;

        for (auto && s : shapes) {
            s.bufferize(vm["buffer"].as<int>());
        }
    }

    //If the user did not specify width, take document width for packing (idem for height)
    Point packerDim(
        (!vm.count("width") || vm["width"].as<int>() == 0) ? docDim.x() : vm["width"].as<int>(),
        (!vm.count("height") ||
         vm["height"].as<int>() == 0) ? docDim.y() : vm["height"].as<int>());

    //Prepacking the shapes
    Merger merger(shapes);
    SimpleTransformer trans(shapes);
    merger.merge(trans.transform());
    // merger.merge(trans.transform());
    

    //Packing the shapes
    Scanline solver(shapes, packerDim);
    solver.solve();

    //Evaluating the quality
    LOG(info) << "Compression rate achieved : " << solver.compressionRatio() << endl;
    cout << solver.debugOutputSVG();

    //Producing the output (sending input file and the option to duplicate
    //Outer::Write(vm["input-file"].as<string>(), vm["dup"].as<bool>(), toPack,
    //       docDim.y(), packerDim.y(), shapes);

    return EXIT_SUCCESS;
}
