#include "Shape.h"

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
}

/**
 * Initializes the boost model with a set of points.
 * (should be a closed path)
 */
void Shape::fillShape(vector<Point>& points) {
    ///TODO
}
