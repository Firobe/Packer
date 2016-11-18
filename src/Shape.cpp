#include <algorithm>
#include <boost/geometry/geometry.hpp>

#include "Shape.hpp"

using namespace std;

/**
 * Defines a comparator for Rings
 */
struct {
	inline bool operator()(const Ring& a, const Ring& b){
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
    _indexP1 = 0;
    _indexP2 = 1;
    _oldP1 = _multiP[0].outer()[0];

    for (unsigned int i = 2 ; i < _multiP[0].outer().size() ; i++)
        if (bg::distance(_oldP1, _multiP[0].outer()[i]) >
                bg::distance(_oldP1, _multiP[0].outer()[_indexP2])) {
            _indexP2 = i;
        }

    _oldP2 = _multiP[0].outer()[_indexP2];
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

