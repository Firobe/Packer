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
	for(Point& p : points)
		bg::append(_ring, p);
	cout << "J'ai créé une shape avec " << points.size() << " points" << endl;
	///TODO points
}
