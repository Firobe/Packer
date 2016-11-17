#include "Shape.hpp"

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
	_multiP.resize(1);
    for (Point& p : points) {
        bg::append(_multiP[0].outer(), p);
    }

    ///TODO points
}

/**
 * Specialisation for Shape.
 */
template <>
void rotate <Shape> (Shape& object, double angle) {
    rotate<MultiPolygon> (object.getMultiP(), angle);
}

/**
 * Specialisation for Shape.
 */
template <>
void translate <Shape> (Shape& object, double x, double y) {
    translate<MultiPolygon> (object.getMultiP(), x, y);
}

