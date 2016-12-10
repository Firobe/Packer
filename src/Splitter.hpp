#ifndef SPLITTER__H
#define SPLITTER__H

#include <vector>

#include "common.hpp"
#include "Shape.hpp"

class Splitter {
private:
    std::vector<Ring> _shapes;
public:
    Splitter(double width, double height) : _shapes() {
        Ring tmp{ {0., 0.}, {width, 0.}, {width, height}, {0., height}, {0., 0.}};
        _shapes.push_back(tmp);
    }
    void split(Point a, Point b);
    void halfSplit(Point a, Point b, Point);
    std::vector<Shape> getShapes() const;
};

#endif
