#ifndef SHAPE__H
#define SHAPE__H

#include <string>
#include <boost/geometry.hpp>
#include <array>
#include "types.h"
#include <boost/geometry/geometries/register/ring.hpp>

/**
 * Shape class. Designed to be
 * carried around by the different
 * modules of the program.
 *
 * Inherits a boost Ring and stores two
 * points at initialization so it can
 * compute its transformation matrix later.
 *
 * Also stores the initial parsed string.
 */
class Shape {
private:
    std::string _tag;
	Ring _ring;
    Point _oldP1, _oldP2;
    int _indexP1, _indexP2;

    void fillShape(std::vector<Point>&);
public:
    Shape(std::string s, std::vector<Point>& p) : _tag(s) {
        fillShape(p);
    }
    std::string getTag() const {
        return _tag;
    }
    std::array<double, 6> getTransMatrix() const;
	Ring& getRing(){ return _ring; }
};

#endif
