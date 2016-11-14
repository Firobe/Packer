#include "Shape.h"
;
using namespace std;

/**
 * Returns [a, b, c, d, e, f] corresponding to the 3*3 matrix :
 * a c e
 * b d f
 * 0 0 1
 * being the transformation matrix of the shape relatively to its
 * starting position.
 */
array<double, 6> Shape::getTransMatrix() const {
    ///TODO
    return array<double, 6> {0, 0, 0, 0, 0, 0};
}

/**
 * Initializes the boost model with a set of points.
 * (should be a closed path)
 *
 * Also initializes _oldP1, P2, _indexP1, P2 which should
 * be two points from the shape as for as possible from each other.
 * (will be used to compute transformation matrix)
 */
void Shape::fillShape(vector<Point>& points) {
    for (Point& p : points) {
        bg::append(_ring, p);
    }

    ///TODO points
}

/**
 * Rotates a shape by a fixed number of
 * degrees (trigonometric).
 */
void Shape::rotate(double angle) {
    Ring r;
    bg::strategy::transform::rotate_transformer<bg::degree, double, 2, 2>
    rotate(angle);
    bg::transform(_ring, r, rotate);
    _ring = r;
}

/**
 * Translates a shape by a fixed vector.
 */
void Shape::translate(double x, double y) {
    Ring r;
    bg::strategy::transform::translate_transformer<double, 2, 2>
    translate(x, y);
    bg::transform(_ring, r, translate);
    _ring = r;
}
