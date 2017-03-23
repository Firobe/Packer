#include <vector>

#include "TheSkyIsTheLimitSolver.hpp"
#include "common.hpp"

using namespace std;

void TheSkyIsTheLimitSolver::preSolve() {
    sort(_shapes.begin(), _shapes.end(), shapeHeightLess);

    // Create the sorted bounding _boxes by decreasing height
    for (unsigned i = 0; i < _shapes.size(); ++i)
        _shapes[i].envelope(_boxes[i]);
}

void TheSkyIsTheLimitSolver::solveBin() {
    // == Stopping Cases ==
    if (_boxes[0].max_corner().y() - _boxes[0].min_corner().y() > Parser::getDims().y()) {
        // STOP, remaining pieces are too tall to fit in any way
        throw invalid_argument("Shape height greater than bin height");
    }

    for (auto && i : _indices) {
        if (_boxes[i].max_corner().x() - _boxes[i].min_corner().x() > Parser::getDims().x())
            throw invalid_argument("Shape width greater than bin width");
    }

    double currY = _binNumber * Parser::getDims().y(), currX = 0, offset;
    offset = _boxes[0].max_corner().y() -
             _boxes[0].min_corner().y(); //height of the working line

    for (list<unsigned>::iterator i = _indices.begin() ; i != _indices.end() ; ++i) {
        if (currX + _boxes[*i].max_corner().x() - _boxes[*i].min_corner().x() >
                Parser::getDims().x()) {
            //if the box doesn't fit in the current line,
            //we create a new line under the first box of the line
            currY += offset;
            currX = 0;
            offset = _boxes[*i].max_corner().y() - _boxes[*i].min_corner().y();

            if (currY + offset > Parser::getDims().y() * (_binNumber + 1)) {
                //if space is missing in current bin,
                //we need to create a new one before resuming packing
                return;
            }
        }

        _shapes[*i].translate(currX - _boxes[*i].min_corner().x(),
                              currY - _boxes[*i].min_corner().y());
        translate<Box>(_boxes[*i], currX - _boxes[*i].min_corner().x(),
                       currY - _boxes[*i].min_corner().y());
        currX += _boxes[*i].max_corner().x() - _boxes[*i].min_corner().x();
        markPacked(i);
    }
}
