#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/strategies/cartesian/area_surveyor.hpp>

#include "Splitter.hpp"
#include "common.hpp"

#define EPSILON 10e-10

using namespace std;

bool sideOfLine(Point a, Point b, Point c) {
    //Droite AB , point C
    return (b.x() - a.x()) * (c.y() - a.y()) - (b.y() - a.y()) * (c.x() - a.x()) > 0;
}

Point intersect(Point a, Point b, Point c, Point d) {
    if (abs(b.x() - a.x()) < EPSILON)
        return Point(a.x(),
                     (d.y() - c.y()) * (a.x() - c.x())
                     / (d.x() - c.x())
                     + c.y());

    //Point d'intersection entre AB et CD
    double rX = ((b.y() - a.y()) * (d.x() - c.x()) * a.x()
                 - (b.x() - a.x()) * (d.x() - c.x()) * a.y()
                 - (d.y() - c.y()) * (b.x() - a.x()) * c.x()
                 + (b.x() - a.x()) * (d.x() - c.x()) * c.y())
                / ((d.x() - c.x()) * (b.y() - a.y())
                   - (d.y() - c.y()) * (b.x() - a.x()));
    double rY = (b.y() - a.y()) * (rX - a.x())
                / (b.x() - a.x())
                + a.y();
    return Point(rX, rY);
}

void Splitter::split(Point a, Point b) {
    vector<Ring> result;

    for (auto && shape : _shapes) {
        bool last = sideOfLine(a, b, shape[0]);
        Ring sup, inf;

        for (unsigned rI = 0 ; rI < shape.size() ; ++rI) {
            //i : shape
            //rI : point index
            if (sideOfLine(a, b, shape[rI]) != last) {
                Point inter = intersect(a, b,
                                        shape[rI - 1], shape[rI]);
                bg::append(sup, inter);
                bg::append(inf, inter);
                last = !last;
            }

            bg::append(last ? sup : inf, shape[rI]);
        }

        bg::correct(sup);
        bg::correct(inf);

        if (!sup.empty()) {
            result.push_back(sup);
        }

        if (!inf.empty()) {
            result.push_back(inf);
        }
    }

    _shapes = result;
}

vector<Shape> Splitter::getShapes() const {
    vector<Shape> result;
    result.reserve(_shapes.size());

    for (unsigned i = 0 ; i < _shapes.size() ; ++i) {
        vector<Ring> tmp(1, _shapes[i]);
        result.emplace_back(tmp, i);
    }

    return result;
}
