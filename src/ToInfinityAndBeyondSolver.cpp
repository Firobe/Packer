#include "ToInfinityAndBeyondSolver.h"
#include <vector>
#include "types.h"

using namespace std;

void ToInfinityAndBeyondSolver::solve() {
    //_shapes[pouet] //contient l'ensemble des objets
    vector<Box> boxes(_shapes.size());
    Point previous, reference;

    for (unsigned i = 0; i < _shapes.size(); i++) {
        bg::envelope(_shapes[i].getRing(), boxes[i]);
    }

    for (unsigned i = 0; i < _shapes.size(); i++) {
        reference = boxes[i].min_corner();
        previous = i != 0 ? boxes[i - 1].max_corner() : Point(0, 0);
        _shapes[i].translate(previous.x() - reference.x(), -reference.y());
        bg::envelope(_shapes[i].getRing(), boxes[i]);
    }

}
