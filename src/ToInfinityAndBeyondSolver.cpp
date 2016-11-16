#include <boost/geometry/algorithms/envelope.hpp>
#include <vector>

#include "ToInfinityAndBeyondSolver.hpp"
#include "types.hpp"

using namespace std;

void ToInfinityAndBeyondSolver::solve() {
    vector<Box> boxes(_shapes.size());
    Point previous, reference;

    //Create the bounding boxes
    for (unsigned i = 0; i < _shapes.size(); i++) {
        bg::envelope(_shapes[i].getRing(), boxes[i]);
    }

    for (unsigned i = 0; i < _shapes.size(); i++) {
        reference = boxes[i].min_corner();
        previous = i != 0 ? boxes[i - 1].max_corner() : Point(0, 0);
        translate<Shape>(_shapes[i], previous.x() - reference.x(), -reference.y());
        translate<Box>(boxes[i], previous.x() - reference.x(), -reference.y());
    }

}

