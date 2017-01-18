#include <vector>

#include <boost/geometry/algorithms/envelope.hpp>

#include "TheSkyIsTheLimit.hpp"
#include "common.hpp"

using namespace std;

void TheSkyIsTheLimitSolver::preSolve() {
    sort(_shapes.begin(), _shapes.end(), shapeHeightLess);

    //Create the bounding boxes
    for (unsigned i = 0; i < _shapes.size(); ++i) {
        bg::envelope(_shapes[i].getMultiP(), _boxes[i]);
    }
}

void TheSkyIsTheLimitSolver::solveBin() {
    Point cursor;
    double currY = _binNumber * _dimensions.y(), currX = 0, offset;
    offset = _boxes[0].max_corner().y() - _boxes[0].min_corner().y();

    for (list<unsigned>::iterator i = _indices.begin() ; i != _indices.end() ; ++i) {
        if (currX + _boxes[*i].max_corner().x() - _boxes[*i].min_corner().x() > _dimensions.x()) {
            currY += offset;
            currX = 0;
            offset = _boxes[*i].max_corner().y() - _boxes[*i].min_corner().y();

            if (currY + offset > _dimensions.y() * (_binNumber + 1)) {
                return;
            }
        }

        translate<Shape>(_shapes[*i], currX - _boxes[*i].min_corner().x() ,
                         currY - _boxes[*i].min_corner().y());
        translate<Box>(_boxes[*i], currX - _boxes[*i].min_corner().x(),
                       currY - _boxes[*i].min_corner().y());
        currX += _boxes[*i].max_corner().x() - _boxes[*i].min_corner().x();
        markPacked(i);
    }
}
