#include <algorithm>
#include <boost/geometry/geometry.hpp>
#include <cmath>

#include "Shape.hpp"

using namespace std;

/**
 * Defines a comparator for Rings
 */
struct {
    inline bool operator()(const Ring& a, const Ring& b) {
        return bg::area(a) < bg::area(b);
    }
} ringLess;

/**
 * Returns [a, b, c, d, e, f] corresponding to the 3*3 matrix :
 * a c e
 * b d f
 * 0 0 1
 * being the transformation matrix of the shape relatively to its
 * starting position.
 */
array<double, 6> Shape::getTransMatrix() const {
    Point newP1 = _multiP[0].outer()[0];
    Point newP2 = _multiP[0].outer()[1];
    //Computing the angle
    double alpha = atan((newP2.y() - newP1.y())
                        / (newP2.x() - newP1.x()))
                   - atan((_oldP2.y() - _oldP1.y())
                          / (_oldP2.x() - _oldP1.x()));
    array<double, 6> result;
    double c, s, x1, y1, x2, y2;
    //Resulting matrix corresponding to the following operations:
    //Translate to origin, rotate of alpha, translate to new position
    c = cos(alpha);
    s = sin(alpha);
    x1 = _oldP1.x();
    y1 = _oldP1.y();
    x2 = newP1.x();
    y2 = newP1.y();
    result[0] = c;
    result[1] = -s;
    result[2] = s;
    result[3] = c;
    result[4] = -x1 * c - y1 * s + x2;
    result[5] = x1 * s - y1 * c + y2;
    return result;
}

/**
 * Initializes the boost model with a set of points.
 * (should be a closed path)
 *
 * Also initializes _oldP1, P2, _indexP1, P2 which should
 * be two points from the shape as for as possible from each other.
 * (will be used to compute transformation matrix)
 */
void Shape::fillShape(vector<Ring>& rings) {
    cerr << "Received " << rings.size() << " Rings for a Shape" << endl;
    //Sorting the rings by increasing area
    sort(rings.begin(), rings.end(), ringLess);

    for (unsigned i = 0 ; i < rings.size() ; i++) {
        bool covered = false;

        for (unsigned j = rings.size() - 1 ; j > i ; j--) {
            if (bg::covered_by(rings[i], rings[j])) {
                covered = true;
                break;
            }
        }

        if (!covered) { //The ring i not covered by anyone
            cerr << "Ring " << i << " is an outer ring" << endl;
            _multiP.resize(_multiP.size() + 1);
            _multiP.back().outer() = rings[i];

            //Discover all the holes of i
            for (unsigned k = 0 ; k < i ; k++)
                if (bg::covered_by(rings[k], rings[i])) {
                    cerr << "-> Ring " << k << " is one if its holes" << endl;
                    _multiP.back().inners().resize(_multiP.back().inners().size() + 1);
                    _multiP.back().inners().back() = rings[k];
                }
        }
    }

    //Storing points for future transformation reference
    _oldP1 = _multiP[0].outer()[0];
    _oldP2 = _multiP[0].outer()[1];
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

