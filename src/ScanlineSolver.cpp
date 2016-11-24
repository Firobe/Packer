#include <boost/geometry/algorithms/envelope.hpp>
#include <vector>
#include <stdexcept>

#include "ScanlineSolver.hpp"
#include "types.hpp"

using namespace std;

void ScanlineSolver::solve() {
    sort(_shapes.begin(), _shapes.end(), shapeHeightLess);

    // Create the sorted bounding _boxes by decreasing height
    for (unsigned i = 0; i < _shapes.size(); i++) {
        bg::envelope(_shapes[i].getMultiP(), _boxes[i]);
    }

    for (unsigned i = 0; i < _shapes.size(); i++){
        _indices.push_back(i);
    }

    ScanlineSolver::solveAux();
}

void ScanlineSolver::solveAux() {
	cerr << "New recursive call to solveAux" << endl;
    // == Stopping Cases ==
    if (_boxes[0].max_corner().y() - _boxes[0].min_corner().y() > _dimensions.y()) {
        // STOP, remaining pieces are too tall to fit in any way
        throw invalid_argument("Shape height higher than bin height");
    }

    for (auto && i : _indices) {
        //  for(unsigned i=0; i<_boxes.size(); i++){
        if (_boxes[i].max_corner().x() - _boxes[i].min_corner().x() > _dimensions.x()) {
            throw invalid_argument("Shape width higher than bin width");
        }
    }

    // =============================================================================
    // We now have a sorted-by-height vector of _boxes, with at least one able to fit
    // =============================================================================
    // Create the dynamic cell matrix
    vector<double> cellW(1);
    vector<double> cellH(1);
    vector<vector<bool>> cellIsEmpty(1, vector<bool>(1, true));
    vector<bool> newCellLine(1); // to divide and add a cell row
    cellW[0] = _dimensions.x();
    cellH[0] = _dimensions.y();

    double shapeWidth;
    double shapeHeight;

    int lastX, lastY;
    double lastH = 0, lastW = 0;

    bool keepLooking;
    list<unsigned>::iterator i = _indices.begin();

    for (; i != _indices.end(); i++) {

        shapeWidth = _boxes[*i].max_corner().x() - _boxes[*i].min_corner().x();
        shapeHeight = _boxes[*i].max_corner().y() - _boxes[*i].min_corner().y();

        keepLooking = true;

        for (unsigned iX = 0; iX < cellW.size() && keepLooking; iX++) {
            for (unsigned iY = 0; iY < cellH.size() &&
                    keepLooking; iY++) { // scan column by column, top to bottom and left to right

                if (cellIsEmpty[iX][iY]) { // First test is to see if the top-left cell is empty
                    lastX = getLastX(cellW, iX, shapeWidth, lastW);
                    lastY = getLastY(cellH, iY, shapeHeight, lastH);

                    if (lastX == -1 || lastY == -1) { // piece goes off the frame
                        continue;
                    }

                    if (allCellsEmpty(cellIsEmpty, iX, lastX, iY,
                                      lastY)) { // If all cells are free to store the box
                        // == Division ==
                        if (lastW != 0) { // If it perfectly fits, no need for division
                            cellIsEmpty.insert(cellIsEmpty.begin() + lastX + 1,
                                               cellIsEmpty[lastX]); // Column copy (with new column same as old)
                            cellW.insert(cellW.begin() + lastX + 1, cellW[lastX] - lastW);   //
                            cellW[lastX] = lastW;          // Width division between the two daughter cells
                        }

                        if (lastH != 0) {
                            for (unsigned j = 0; j < cellIsEmpty.size(); j++) {
                                cellIsEmpty[j].insert(cellIsEmpty[j].begin() + lastY + 1, cellIsEmpty[j][lastY]);
                            }

                            cellH.insert(cellH.begin() + lastY + 1, cellH[lastY] - lastH);
                            cellH[lastY] = lastH;
                        }

                        // == Filling cells ==
                        for (int jx = iX; jx <= lastX; jx++) {
                            for (int jy = iY; jy <= lastY; jy++) {
                                cellIsEmpty[jx][jy] = false;
                            }
                        }

                        // == translation ==
                        translate<Shape>(_shapes[*i], getLenFromIndex(cellW, iX) - _boxes[*i].min_corner().x(),
                                         getLenFromIndex(cellH, iY) - _boxes[*i].min_corner().y() + _dimensions.y() * _binNumber);
                        translate<Box>(_boxes[*i], getLenFromIndex(cellW, iX) - _boxes[*i].min_corner().x(),
                                       getLenFromIndex(cellH, iY) - _boxes[*i].min_corner().y() + _dimensions.y() * _binNumber);

                        // We're done here, going onto next piece
                        cerr << "Je suis " << *i << " et je vais me supprimer du tableau" << endl;
                        i = _indices.erase(i);
						i--;
                        keepLooking = false;
                    }
                }
            }
        }
    }

    if (_indices.size() >
            0) { // recursive call on failed packing (new frame under this one)
        _binNumber++;
        ScanlineSolver::solveAux();
    }
}

int getLastX(vector<double>& cellW, unsigned iX, double shapeW, double& plastW) {
    double w = shapeW;

    while (cellW[iX] < w) {
        w -= cellW[iX];
        iX++;

        if (iX >= cellW.size()) { // piece goes off the frame
            return -1;
        }
    }

    plastW = w; // Last width is keep (to divide the cell)
    return iX;
}


int getLastY(vector<double>& cellH, unsigned iY, double shapeH, double& plastH) {
    double h = shapeH;

    while (cellH[iY] < h) {
        h -= cellH[iY];
        iY++;

        if (iY >= cellH.size()) { // piece goes off the frame
            return -1;
        }
    }

    plastH = h; // Last height is keep (to divide the cell)
    return iY;
}



bool allCellsEmpty(vector<vector<bool>>& cellIsEmpty, unsigned iX, int lastX, unsigned iY,
                   int lastY) {
    for (int x = iX; x <= lastX; x++) {
        for (int y = iY; y <= lastY; y++) {
            if (cellIsEmpty[x][y] == false) {
                return false;
            }
        }
    }
    return true;
}

double getLenFromIndex(vector<double>& lengthVector, unsigned index) {
    double length = 0;

    for (unsigned i = 0; i < index; i++) {
        length += lengthVector[i];
    }
    return length;
}


