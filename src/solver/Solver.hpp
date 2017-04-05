#ifndef SOLVER__H
#define SOLVER__H

#include <string>
#include <algorithm>
#include <list>
#include <sstream>

#include "Parser.hpp"
#include "Layout.hpp"
#include "common.hpp"

#define SPACE_COEF (1.2) //Spacing between bins

/**
 * Solver class. A class
 * as a base for solver algorithms.
 *
 * An inherited class can redefine :
 * 	preSolve : operations to be performed before solving (sorting, ...)
 * 	solveBin : move the shapes to pack a single bin. Is called repeatedly until there is no shape to be packed.
 */
class Solver {
protected:
    Layout& _shapes; //Shapes to be packed
    unsigned _binNumber; //Current number of bins
    std::list<unsigned> _indices; //List of the indices in _shapes not packed yet

    /**
     * Removes the current element in _indices and sets updated iterator
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
    Solver(Layout& s, const std::vector<Parameter>&) : _shapes(s),
        _binNumber(0) {}
    void solve();
    virtual ~Solver() {}
    unsigned getBinNb() {
        return _binNumber;
    }
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
    Box ea, eb;
    a.envelope(ea);
    b.envelope(eb);
    return boxHeightLess(ea, eb);
}

#endif
