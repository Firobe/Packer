#include <sstream>
#include <string>
#include <numeric>

#include "Solver.hpp"
#include "Display.hpp"
#include "Log.hpp"

bool Solver::generalStop = false;

using namespace std;

void Solver::solve() {
    LOG(info) << "Packing shapes..." << endl;
    _indices.resize(_shapes.size());
    iota(_indices.begin(), _indices.end(), 0);
    //_indices is now initialized to {1, 2, ... _shapes.size() - 1 }
    LOG(info) << "Pre-solving..." << endl;
    preSolve();
    LOG(info) << "Solving..." << endl;
    Display::Text("Solving...");

    //While there are shapes left, fill bins individually
    while (!_indices.empty()) {
        LOG(info) << "Bin " << _binNumber + 1
                  << " : " << _indices.size() << " shapes left..." << endl;
        solveBin();
        _binNumber++;
    }

    LOG(info) << "Successfully packed "
              << _shapes.size() << " shapes in " << _binNumber << " bins." << endl;
}
