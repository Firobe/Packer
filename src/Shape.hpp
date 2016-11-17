#ifndef SHAPE__H
#define SHAPE__H

#include <boost/geometry/algorithms/transform.hpp>
#include <boost/geometry/strategies/transform/matrix_transformers.hpp>

#include <string>
#include <array>

#include "types.hpp"

/**
 * Shape class. Designed to be
 * carried around by the different
 * modules of the program.
 *
 * Inherits a boost MultiP and stores two
 * points at initialization so it can
 * compute its transformation matrix later.
 *
 * Also stores the initial parsed string.
 */
class Shape {
private:
    std::string _tag;
    MultiPolygon _multiP;
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
    MultiPolygon& getMultiP() {
        return _multiP;
    }
};

/**
 * Rotates a boost primitive by a fixed number of
 * degrees (trigonometric).
 */
template <typename T>
void rotate(T& object, double angle) {
    T r;
    bg::strategy::transform::rotate_transformer<bg::degree, double, 2, 2>
    rotate(angle);
    bg::transform(object, r, rotate);
    object = r;
}

template <>
void rotate <Shape> (Shape& object, double angle);


/**
 * Translates a boost primitive by a fixed vector.
 */
template <typename T>
void translate(T& object, double x, double y) {
    T r;
    bg::strategy::transform::translate_transformer<double, 2, 2>
    translate(x, y);
    bg::transform(object, r, translate);
    object = r;
}

template <>
void translate <Shape> (Shape& object, double x, double y);

#endif
