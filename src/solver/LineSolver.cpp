#include "LineSolver.hpp"

using namespace std;

void LineSolver::solveBin() {
    vector<Box> boxes(_shapes.size());
    Point previous, reference;

    //Create the bounding boxes
    for (unsigned i = 0; i < _shapes.size(); ++i)
        _shapes[i].envelope(boxes[i]);

    for (unsigned i = 0; i < _shapes.size(); ++i) {
        reference = boxes[i].min_corner();
        previous = (i != 0)  ? boxes[i - 1].max_corner() : Point(0, 0);
        _shapes[i].translate(previous.x() - reference.x(), - reference.y());
        translate<Box>(boxes[i], previous.x() - reference.x(), - reference.y());
    }

    _indices.clear();
}

