#ifndef SOLVER__H
#define SOLVER__H

#include <boost/geometry/algorithms/envelope.hpp>
#include <rapidxml_ns/rapidxml_ns.hpp>
#include <string>
#include <algorithm>
#include <list>
#include <sstream>

#include "Shape.hpp"

#define SPACE_COEF (1.2)

/**
 * Solver class. An abstract class
 * as a base for solver algorithms.
 *
 * An inherited algorithm only has to
 * implement the solve() method, which
 * should modify _shapes and nothing
 * else.
 */
class Solver {
protected:
    std::vector<Shape>& _shapes;
    int _binNumber;
    Point _dimensions;
    std::list<unsigned> _indices;
    void markPacked(std::list<unsigned>::iterator& i) {
        i = --_indices.erase(i);
    }
    virtual void preSolve() {}
    virtual void solveBin() {
        _indices.clear();
    }
public:
    Solver(std::vector<Shape>& s, Point p) : _shapes(s), _binNumber(0), _dimensions(p) {}
    void solve();
    std::string debugOutputSVG() const;
};

/**
 * Defines a comparator for boxes
 */
inline bool boxHeightLess(const Box& a, const Box& b) {
    return (a.max_corner().y() - a.min_corner().y()) >=
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
