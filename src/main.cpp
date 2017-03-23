#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>

#include "Log.hpp"
#include "Outer.hpp"
#include "Parser.hpp"
#include "Display.hpp"
#include "CloseEnough.hpp"

namespace po = boost::program_options;
using namespace std;

void parseCommandLine(int, char**, po::variables_map&);
void signalHandler(int);

int main(int argc, char** argv) {
    LOG(info) << "SUPER PACKER 3000\n===================" << endl;
    po::variables_map vm; //Parameters container

    try {
        parseCommandLine(argc, argv, vm);
    }
    catch (exception& e) {
        LOG(fatal) << "Error : " << e.what() << endl;
        return EXIT_FAILURE;
    }

    srand(vm["seed"].as<unsigned>());
    LOG(info) << "Seed : " << vm["seed"].as<unsigned>() << endl;
    string toDo;

    if (vm.count("confString"))
        toDo = vm["confString"].as<string>();
    else {
        ifstream f(vm["conf"].as<string>());
        toDo.assign((istreambuf_iterator<char>(f)), (istreambuf_iterator<char>()));
    }

    //Will contain the IDs the solver will pack
    vector<string> toPack;

    //If the user selected some shapes, add them to toPack
    if (vm.count("id"))
        toPack = vm["id"].as<vector<string>>();

    //Parsing input file, sending to the parser the ids of the shapes we want to keep
    vector<Shape> shapes = Parser::Parse(vm["input-file"].as<string>(), toPack);

    if (vm["display"].as<bool>())
#ifdef ENABLE_DISPLAY
        Display::Init(shapes);

#else
        LOG(warning) <<
                     "Warning : project was not built with display support, so no display for you !" << endl;
#endif

    //If nothing was selected, fill toPack with every parsed ID
    if (!vm.count("id"))
        LOG(info) << "Will pack *all* shapes." << endl;

    for (auto && s : shapes)
        toPack.push_back(s.getIdentifier());

    //Apply a buffer to the shapes
    LOG(info) << "Buffering shapes..." << endl;
    #pragma omp parallel for schedule(dynamic, 1)

    for (unsigned i = 0 ; i < shapes.size() ; ++i)
        shapes[i].bufferize(vm["buffer"].as<double>());

    //If the user did not specify width, take document width for packing (idem for height)
    Parser::setDims(Point(
                        (vm["width"].as<int>() == 0) ? Parser::getDims().x() : vm["width"].as<int>(),
                        (vm["height"].as<int>() == 0) ? Parser::getDims().y() : vm["height"].as<int>()));
    //signal(SIGINT, signalHandler);
    //MAIN PROCESSING
    CE_Parser parser(shapes);
    auto begin = toDo.begin(), end = toDo.end();
    bool success = phrase_parse(begin, end, parser, ascii::space);

    if (success and begin == end)
        LOG(info) << "CloseEnough successfully processed !" << endl;
    else
        throw runtime_error("CloseEnough configuration is invalid (see parsing error for details)");

    //Evaluating the quality
    LOG(info) << "Compression rate achieved : " << compressionRatio(shapes) << endl;

    //Producing the output (sending input file and the option to duplicate
    if (vm["debug"].as<bool>())
        cout << debugOutputSVG(shapes);
    else
        Outer::Write(vm["input-file"].as<string>(), vm["dup"].as<bool>(), toPack, shapes);

    return EXIT_SUCCESS;
}

void parseCommandLine(int argc, char** argv, po::variables_map& vm) {
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
    ("conf,c", po::value<string>()->default_value("default.ce"),
     "CloseEnough configuration file (refer to documentation)")
    ("confString,cs", po::value<string>(), "CloseEnough string (used instead of file)")
    ("debug", po::bool_switch()->default_value(false),
     "Produce debug SVG instead of real one")
    ("buffer", po::value<double>()->default_value(0.),
     "minimal distance between packed items (px)")
    ("display,D", po::value<bool>()->default_value(false),
     "Enable real time output (project must be compiled with ENABLE_DISPLAY=1")
    ("seed,s", po::value<unsigned>()->default_value(time(0)), "Seed used by the RNG");
    po::positional_options_description p; //Used to indicate input file without --input-file
    p.add("input-file", -1);
    //Effectively parse the command line
    po::store(po::command_line_parser(argc, argv).
              options(desc).positional(p).allow_unregistered().run(), vm);

    if (vm.count("help")) {
        cerr << desc << endl;
        exit(EXIT_SUCCESS);
    }

    //Check parsing errors (required parameters, ...)
    po::notify(vm);
}

void signalHandler(int) {
    cout << "User interruption received. Will stop as soon as possible" << endl;
}
