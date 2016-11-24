#ifndef SOLVER__H
#define SOLVER__H

#include <boost/geometry/algorithms/envelope.hpp>
#include <rapidxml_ns/rapidxml_ns.hpp>
#include <string>
#include <algorithm>
#include <sstream>


#include "Shape.hpp"


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
public:
    Solver(std::vector<Shape>& s, Point p) : _shapes(s), _binNumber(0), _dimensions(p) {}
    void solve() = delete;
    std::string debugOutputSVG();
};

/**
 * Defines a comparator for boxes
 */
struct {
    inline bool operator()(const Box& a, const Box& b) {
        return (a.max_corner().y() - a.min_corner().y()) >=
               (b.max_corner().y() - b.min_corner().y());

    }
} boxHeightLess;

/**
 * Defines a comparator for Shapes (height of the enveloppe)
 */
struct {
    inline bool operator()(const Shape& a, const Shape& b) {
        Box ea;
        bg::envelope(a.getMultiP(), ea);
        Box eb;
        bg::envelope(b.getMultiP(), eb);
        return boxHeightLess(ea, eb);
    }
} shapeHeightLess;

#endif
