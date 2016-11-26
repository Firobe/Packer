#include <boost/geometry/algorithms/envelope.hpp>
#include <vector>

#include "TheSkyIsTheLimitSolver.hpp"
#include "types.hpp"

using namespace std;



void TheSkyIsTheLimitSolver::solve() {
    vector<Box> boxes(_shapes.size());
    Point cursor;
    double currX = 0, currY = 0, offset;
    sort(_shapes.begin(), _shapes.end(), shapeHeightLess);

    //Create the bounding boxes
    for (unsigned i = 0; i < _shapes.size(); i++) {
        bg::envelope(_shapes[i].getMultiP(), boxes[i]);
    }

    offset = boxes[0].max_corner().y() - boxes[0].min_corner().y();

    for (unsigned i = 0; i < _shapes.size(); i++) {
        if (currX + boxes[i].max_corner().x() - boxes[i].min_corner().x() > _dimensions.x()) {
            currY += offset;
            currX = 0;
            offset = boxes[i].max_corner().y() - boxes[i].min_corner().y();

            if (currY + offset > _dimensions.y() * (_binNumber + 1)) {
                _binNumber++;
                currY = _binNumber * _dimensions.y();
            }
        }

        translate<Shape>(_shapes[i], currX - boxes[i].min_corner().x() ,
                         currY - boxes[i].min_corner().y());
        translate<Box>(boxes[i], currX - boxes[i].min_corner().x(),
                       currY - boxes[i].min_corner().y());
        currX += boxes[i].max_corner().x() - boxes[i].min_corner().x();
    }
}


