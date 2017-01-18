#include <stdexcept>
#include <vector>

#include <boost/geometry/algorithms/envelope.hpp>

#include "Scanline.hpp"
#include "Log.hpp"
#include "common.hpp"

using namespace std;

void Scanline::preSolve() {
    sort(_shapes.begin(), _shapes.end(), shapeHeightLess);

    // Create the sorted bounding _boxes by decreasing height
    for (unsigned i = 0; i < _shapes.size(); ++i) {
        bg::envelope(_shapes[i].getMultiP(), _boxes[i]);
    }
}

void Scanline::solveBin() {
    // == Stopping Cases ==
    if (_boxes[0].max_corner().y() - _boxes[0].min_corner().y() > _dimensions.y()) {
        // STOP, remaining pieces are too tall to fit in any way
        throw invalid_argument("Shape height greater than bin height");
    }

    for (auto && i : _indices) {
        if (_boxes[i].max_corner().x() - _boxes[i].min_corner().x() > _dimensions.x()) {
            throw invalid_argument("Shape width greater than bin width");
        }
    }

    // =============================================================================
    // We now have a sorted-by-height vector of _boxes, with at least one able to fit
    // =============================================================================
    // Create the dynamic cell matrix
    vector<double> cellW(1);
    vector<double> cellH(1);
    vector<vector<bool>> cellIsEmpty(1, vector<bool>(1, true)); //2-dim boolean matrix
    vector<bool> newCellLine(1); // to divide and add a cell row
    cellW[0] = _dimensions.x();
    cellH[0] = _dimensions.y();
    double shapeWidth;
    double shapeHeight;
    int lastX, lastY;
    double lastH = 0, lastW = 0;
    bool keepLooking;

    // Iterates on yet-to-be-processed shapes
    for (list<unsigned>::iterator i = _indices.begin(); i != _indices.end(); ++i) {
        LOG(debug) << ".";
        shapeWidth = _boxes[*i].max_corner().x() - _boxes[*i].min_corner().x();
        shapeHeight = _boxes[*i].max_corner().y() - _boxes[*i].min_corner().y();
        keepLooking = true;

        // scan column by column, top to bottom and left to right
        for (unsigned iX = 0; iX < cellW.size() && keepLooking; ++iX) {
            for (unsigned iY = 0; iY < cellH.size() && keepLooking; ++iY) {

                if (cellIsEmpty[iX][iY]) { // First test is to see if the top-left cell is empty
                    lastX = getLast(cellW, iX, shapeWidth, lastW);
                    lastY = getLast(cellH, iY, shapeHeight, lastH);

                    if (lastX == -1 || lastY == -1) { // piece goes off the frame
                        continue;
                    }

                    if (allCellsEmpty(cellIsEmpty, iX, lastX, iY,
                                      lastY)) { // If all cells are free to store the box
                        // == Division ==
                        if (lastW > PRECISION &&
                                cellW[lastX] - lastW > PRECISION) { // If it perfectly fits, no need for division
                            cellIsEmpty.insert(cellIsEmpty.begin() + lastX + 1,
                                               cellIsEmpty[lastX]); // Column copy (with new column same as old)
                            cellW.insert(cellW.begin() + lastX + 1, cellW[lastX] - lastW);   //
                            cellW[lastX] = lastW;          // Width division between the two daughter cells
                        }

                        if (lastH > PRECISION && cellH[lastY] - lastH > PRECISION) {
                            for (unsigned j = 0; j < cellIsEmpty.size(); j++) {
                                cellIsEmpty[j].insert(cellIsEmpty[j].begin() + lastY + 1, cellIsEmpty[j][lastY]);
                            }

                            cellH.insert(cellH.begin() + lastY + 1, cellH[lastY] - lastH);
                            cellH[lastY] = lastH;
                        }

                        // == Filling cells ==
                        for (int jx = iX; jx <= lastX; ++jx) {
                            for (int jy = iY; jy <= lastY; ++jy) {
                                cellIsEmpty[jx][jy] = false;
                            }
                        }

                        // == translation ==
                        translate<Shape>(_shapes[*i], getLenFromIndex(cellW, iX) - _boxes[*i].min_corner().x(),
                                         getLenFromIndex(cellH, iY) - _boxes[*i].min_corner().y() + _dimensions.y() * _binNumber *
                                         SPACE_COEF);
                        translate<Box>(_boxes[*i], getLenFromIndex(cellW, iX) - _boxes[*i].min_corner().x(),
                                       getLenFromIndex(cellH, iY) - _boxes[*i].min_corner().y() + _dimensions.y() * _binNumber *
                                       SPACE_COEF);
                        // We're done here, going onto next piece
                        markPacked(i);
                        keepLooking = false;
                    }
                }
            }
        }
    }
}

int Scanline::getLast(const vector<double>& cells, unsigned i, double length,
                            double& plast) const {
    while (cells[i] + PRECISION < length) {
        length -= cells[i];
        i++;

        if (i >= cells.size()) { // piece goes off the frame
            return -1;
        }
    }

    plast = length; // Last width/height is kept (to divide the cell)
    return i;
}

bool Scanline::allCellsEmpty(const vector<vector<bool>>& cellIsEmpty, unsigned iX,
                                   int lastX, unsigned iY,
                                   int lastY) const {
    for (int x = iX; x <= lastX; ++x) {
        for (int y = iY; y <= lastY; ++y) {
            if (cellIsEmpty[x][y] == false) {
                return false;
            }
        }
    }

    return true;
}

void Scanline::printAll(vector<vector<bool>>& cellIsEmpty, vector<double> cellW,
                              vector<double>& cellH) {
    LOG(debug) << "============\nCELL MATRIX";

    for (auto && x : cellIsEmpty) {
        LOG(debug) << endl;

        for (auto && y : x) {
            LOG(debug) << y;
        }
    }

    LOG(debug) << endl << "CellW" << endl;

    for (auto && x : cellW) {
        LOG(debug) << x << " ; ";
    }

    LOG(debug) << endl << "CellH" << endl;

    for (auto && x : cellH) {
        LOG(debug) << x << " ; ";
    }

    LOG(debug) << endl;
}

double Scanline::getLenFromIndex(const vector<double>& lengthVector,
                                       unsigned index) const {
    double length = 0;

    for (unsigned i = 0; i < index; ++i) {
        length += lengthVector[i];
    }

    return length;
}