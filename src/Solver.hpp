#ifndef SOLVER__H
#define SOLVER__H

#include <string>
#include <algorithm>
#include <list>
#include <sstream>

#include <boost/geometry/algorithms/envelope.hpp>

#include "Shape.hpp"

#define SPACE_COEF (1.2)

/**
 * Solver class. A class
 * as a base for solver algorithms.
 * Do not move the shapes.
 *
 * An inherited class can redefine :
 * 	preSolve : operations to be performed before solving (sorting, ...)
 * 	solveBin : move the shapes to pack a single bin. Is called repeatedly until there is no shape to be packed.
 */
class Solver {
protected:
    std::vector<Shape>& _shapes; //Shapes to be packed
    unsigned _binNumber; //Current number of bins
    Point _dimensions; //Document dimensions
    std::list<unsigned> _indices; //List of the indices in _shapes not packed yet

    /**
     * Removes the current element in _indices and returns updated iterator
     */
    void markPacked(std::list<unsigned>::iterator& i) {
        i = --_indices.erase(i);
    }

    virtual void preSolve() {}

    /**
     * Apply a packing algorithm to
     * fill a single bin.
     */
    virtual void solveBin() {
        _indices.clear();
    }

public:
    Solver(std::vector<Shape>& s, Point p) : _shapes(s), _binNumber(0), _dimensions(p) {}
    void solve();
    double compressionRatio() const;
    std::string debugOutputSVG() const;
};

/**
 * Defines a comparator for boxes
 */
inline bool boxHeightLess(const Box& a, const Box& b) {
    return (a.max_corner().y() - a.min_corner().y()) >
           (b.max_corner().y() - b.min_corner().y());
}

/**
 * Defines a comparator for Shapes (height of the enveloppe)
 */
inline bool shapeHeightLess(const Shape& a, const Shape& b) {
    Box ea {};
    bg::envelope(a.getMultiP(), ea);
    Box eb {};
    bg::envelope(b.getMultiP(), eb);
    return boxHeightLess(ea, eb);
}

#endif
