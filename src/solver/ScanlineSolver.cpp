#include <stdexcept>
#include <vector>

#include "ScanlineSolver.hpp"
#include "Log.hpp"
#include "common.hpp"
#include "Display.hpp"

#define PRECISION 10e-3

using namespace std;

void ScanlineSolver::preSolve() {
    for (unsigned i = 0; i < _shapes.size(); ++i)
        rotateToBestAngle(_shapes[i]);

    sort(_shapes.begin(), _shapes.end(), shapeHeightLess);

    // Create the sorted bounding _boxes by decreasing height
    for (unsigned i = 0; i < _shapes.size(); ++i)
        _shapes[i].envelope(_boxes[i]);
}

void ScanlineSolver::solveBin() {
    for (auto && i : _indices) {
        if ((_boxes[i].max_corner().x() - _boxes[i].min_corner().x() > Parser::getDims().x()
                && _boxes[i].max_corner().y() - _boxes[i].min_corner().y() > Parser::getDims().x())
                ||
                (_boxes[i].max_corner().y() - _boxes[i].min_corner().y() > Parser::getDims().y()
                 && _boxes[i].max_corner().x() - _boxes[i].min_corner().x() > Parser::getDims().y()))
            throw invalid_argument("Shape too big to fit");
    }

    // =============================================================================
    // We now have a sorted-by-height vector of _boxes, with at least one able to fit
    // =============================================================================
    // Create the dynamic cell matrix
    vector<double> cellW(1);
    vector<double> cellH(1);
    vector<vector<bool>> cellIsEmpty(1, vector<bool>(1, true)); //2-dim boolean matrix
    vector<bool> newCellLine(1); // to divide and add a cell row
    cellW[0] = Parser::getDims().x();
    cellH[0] = Parser::getDims().y();
    double shapeWidth;
    double shapeHeight;
    int lastX, lastY;
    double lastH = 0, lastW = 0;
    bool keepLooking;

    // Iterates on yet-to-be-processed shapes
    for (int clounk = 0; clounk < 2; clounk++) {
        for (list<unsigned>::iterator i = _indices.begin(); i != _indices.end(); ++i) {
            LOG(debug) << ".";
            shapeWidth = _boxes[*i].max_corner().x() - _boxes[*i].min_corner().x();
            shapeHeight = _boxes[*i].max_corner().y() - _boxes[*i].min_corner().y();
            keepLooking = true;

            // scan column by column, top to bottom and left to right
            for (unsigned iX = 0; iX < cellW.size() && keepLooking; ++iX) {
                for (unsigned iY = 0; iY < cellH.size() && keepLooking; ++iY) {
                    if (cellIsEmpty[iX][iY]) { // First test is to see if the top-left cell is empty
                        if (clounk == 0) {
                            lastX = getLast(cellW, iX, shapeWidth, lastW);
                            lastY = getLast(cellH, iY, shapeHeight, lastH);
                        }
                        else {
                            lastX = getLast(cellW, iX, shapeHeight, lastW);
                            lastY = getLast(cellH, iY, shapeWidth, lastH);
                        }

                        if (lastX == -1 || lastY == -1)   // piece goes off the frame
                            continue;

                        if (allCellsEmpty(cellIsEmpty, iX, lastX, iY,
                                          lastY)) { // If all cells are free to store the box
                            // == Division ==
                            if (lastW > PRECISION &&
                                    cellW[lastX] - lastW > PRECISION) { // Need for division only if it doesn't fit exactly
                                cellIsEmpty.insert(cellIsEmpty.begin() + lastX + 1,
                                                   cellIsEmpty[lastX]); // Column copy (with new column same as old)
                                cellW.insert(cellW.begin() + lastX + 1, cellW[lastX] - lastW);
                                cellW[lastX] = lastW;          // Width division between the two daughter cells
                            }

                            if (lastH > PRECISION && cellH[lastY] - lastH > PRECISION) {
                                for (unsigned j = 0; j < cellIsEmpty.size(); j++)
                                    cellIsEmpty[j].insert(cellIsEmpty[j].begin() + lastY + 1, cellIsEmpty[j][lastY]);

                                cellH.insert(cellH.begin() + lastY + 1, cellH[lastY] - lastH);
                                cellH[lastY] = lastH;
                            }

                            // == Filling cells ==
                            for (int jx = iX; jx <= lastX; ++jx) {
                                for (int jy = iY; jy <= lastY; ++jy)
                                    cellIsEmpty[jx][jy] = false;
                            }

                            // == rotation/translation ==
                            if (clounk == 1) {
                                _shapes[*i].rotate(90);
                                _shapes[*i].envelope(_boxes[*i]);
                            }

                            _shapes[*i].translate(getLenFromIndex(cellW, iX) - _boxes[*i].min_corner().x(),
                                                  getLenFromIndex(cellH, iY) - _boxes[*i].min_corner().y() + Parser::getDims().y() *
                                                  _binNumber *
                                                  SPACE_COEF);
                            translate<Box>(_boxes[*i], getLenFromIndex(cellW, iX) - _boxes[*i].min_corner().x(),
                                           getLenFromIndex(cellH, iY) - _boxes[*i].min_corner().y() + Parser::getDims().y() *
                                           _binNumber *
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
}

int ScanlineSolver::getLast(const vector<double>& cells, unsigned i, double length,
                            double& plast) const {
    while (cells[i] + PRECISION < length) {
        length -= cells[i];
        i++;

        if (i >= cells.size())   // piece goes off the frame
            return -1;
    }

    plast = length; // Last width/height is kept (to divide the cell)
    return i;
}

bool ScanlineSolver::allCellsEmpty(const vector<vector<bool>>& cellIsEmpty, unsigned iX,
                                   int lastX, unsigned iY,
                                   int lastY) const {
    for (int x = iX; x <= lastX; ++x)
        for (int y = iY; y <= lastY; ++y)
            if (cellIsEmpty[x][y] == false)
                return false;

    return true;
}

void ScanlineSolver::printAll(vector<vector<bool>>& cellIsEmpty, vector<double> cellW,
                              vector<double>& cellH) {
    cerr << "============\nCELL MATRIX";

    for (auto && x : cellIsEmpty) {
        cerr << endl;

        for (bool y : x)
            cerr << y;
    }

    cerr << endl << "CellW" << endl;

    for (auto && x : cellW)
        cerr << x << " ; ";

    cerr << endl << "CellH" << endl;

    for (auto && x : cellH)
        cerr << x << " ; ";

    cerr << endl;
}

double ScanlineSolver::getLenFromIndex(const vector<double>& lengthVector,
                                       unsigned index) const {
    double length = 0;

    for (unsigned i = 0; i < index; ++i)
        length += lengthVector[i];

    return length;
}
