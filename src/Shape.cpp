#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include <boost/geometry/algorithms/buffer.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/strategies/cartesian/area_surveyor.hpp>
#include <boost/geometry/algorithms/covered_by.hpp>
#include <boost/geometry/strategies/agnostic/point_in_poly_winding.hpp>
#include <boost/geometry/strategies/agnostic/relate.hpp>
#include <boost/geometry/algorithms/num_points.hpp>

#include "Shape.hpp"
#include "Log.hpp"
#include "Parser.hpp"

using namespace std;

/**
 * Defines a comparator for Rings
 */
inline bool ringLess(const Ring& a, const Ring& b) {
    return bg::area(a) < bg::area(b);
}

/**
 * Point stream operator
 */
std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << bg::wkt(p);
    return os;
}


/**
 * Returns [a, b, c, d, e, f] corresponding to the 3*3 matrix :
 * a c e
 * b d f
 * 0 0 1
 * being the transformation matrix of the shape relatively to its
 * starting position.
 */
array<double, 6> Shape::getTransMatrix() const {
    Point newP1 = _multiP[0].outer()[_indexP1];
    Point newP2 = _multiP[0].outer()[_indexP2];
    double c, s, x1, y1, x2, y2, n1, n2;
    //Normalize vectors (x1, y1), (x2, y2)
    n1 = bg::distance(_oldP1, _oldP2);
    n2 = bg::distance(newP1, newP2);

    if (!floatEqual(n1, n2, 10e-6)) {
        throw runtime_error("Point order was modified or the shape was scaled");
    }

    x1 = (_oldP2.x() - _oldP1.x()) / n1;
    y1 = (_oldP2.y() - _oldP1.y()) / n1;
    x2 = (newP2.x() - newP1.x()) / n2;
    y2 = (newP2.y() - newP1.y()) / n2;
    //Computing cos & sin with dot products
    c = x1 * x2 + y1 * y2;
    s = x1 * y2 - x2 * y1;
    //Resulting matrix corresponding to the following operations:
    //Translate to origin, rotate, translate to new position
    array<double, 6> result;
    result[0] = c;
    result[1] = s;
    result[2] = -s;
    result[3] = c;
    result[4] = - _oldP1.x() * c + _oldP1.y() * s + newP1.x();
    result[5] = - _oldP1.x() * s - _oldP1.y() * c + newP1.y();
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
    LOG(debug) << "Received " << rings.size() << " Rings for a Shape" << endl;
    //Sorting the rings by increasing area
    sort(rings.begin(), rings.end(), ringLess);

    for (unsigned i = 0 ; i < rings.size() ; ++i) {
        bool covered = false;

        //We can search only for rings after i thanks to the ring sorting
        for (unsigned j = rings.size() - 1 ; j > i ; --j) {
            if (bg::covered_by(rings[i], rings[j])) {
                covered = true;
                break;
            }
        }

        if (!covered) { //The ring i not covered by anyone
            LOG(debug) << "Ring " << i << " is an outer ring" << endl;
            //Add a new polygon to the MultiPolygon
            _multiP.resize(_multiP.size() + 1);
            //The ring is the outer ring of the new polygon
            _multiP.back().outer() = rings[i];

            //Discover all the holes of i
            //We can search only for rings before i thanks to the ring sorting
            for (unsigned k = 0 ; k < i ; ++k)
                if (bg::covered_by(rings[k], rings[i])) {
                    LOG(debug) << "  -> Ring " << k << " is one if its holes" << endl;
                    //Add an inner ring to our polygon
                    _multiP.back().inners().resize(_multiP.back().inners().size() + 1);
                    //k is that inner ring
                    _multiP.back().inners().back() = rings[k];
                }
        }
    }

    setOld();
}

/**
 * Change shapes by adding a buffer
 * (of <buffer>+BEZIER_TOLERANCE px) around each of them.
 * _oldP1, P2, indexP1, P2 are also recalculated according to the new multiPolygon
 */
void Shape::bufferize(double buffer) {
    /*
     * Buffering at least the interpolation maximal deviation
     * With this we guarantee that there is NO intersection between shapes (or at least it should)
     * due to the interpolation error.
     */
    buffer += BEZIER_TOLERANCE;
    // Declare strategies
    static bg::strategy::buffer::distance_symmetric<double> distance_strategy(buffer);
    static bg::strategy::buffer::join_miter join_strategy(
        2.); //Points will be located to at most 2 * buffer
    static bg::strategy::buffer::end_flat end_strategy;
    static bg::strategy::buffer::point_square circle_strategy;
    static bg::strategy::buffer::side_straight side_strategy;
    MultiPolygon result;
    // Create the buffer of a multi polygon
    bg::buffer(_multiP, result,
               distance_strategy, side_strategy,
               join_strategy, end_strategy, circle_strategy);
    _multiP = result;
    LOG(debug) << "Number of points : " << bg::num_points(_multiP) << endl;
    setOld();
}

/**
 * Store points for future transformation reference.
 * Find two points that maximize distance between them.
 * The first point is currently always at index 0.
 * Points are only concidered in the first Polygon of the shape
 */
void Shape::setOld() {
    _indexP1 = 0;
    _indexP2 = 0;

    for (unsigned i = 1 ; i < _multiP[0].outer().size() ; ++i)
        if (bg::distance(_multiP[0].outer()[_indexP1], _multiP[0].outer()[i]) >
                bg::distance(_multiP[0].outer()[_indexP1], _multiP[0].outer()[_indexP2])) {
            _indexP2 = i;
        }

    _oldP1 = _multiP[0].outer()[_indexP1];
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


/**
 * Rotates object so that its retangular bounding box
 * will be of minimal area
 * Explores a range of 90 degrees with a fixed step
 */
void rotateToBestAngle(Shape& object) {
    const double ANGLE_MAX = 90.0;
    const double ANGLE_STEP = 0.2;
    double bestAngle, currAngle;
    double bestArea, currArea;
    Box currBox;
    bestAngle = 0.0;
    bg::envelope(object.getMultiP(), currBox);
    bestArea = bg::area(currBox);
    currAngle = 0.0;

    while (currAngle <= ANGLE_MAX) {
        rotate<Shape>(object, ANGLE_STEP);
        bg::envelope(object.getMultiP(), currBox);
        currArea = bg::area(currBox);

        if (currArea < bestArea) {
            bestArea = currArea;
            bestAngle = currAngle;
        }

        currAngle += ANGLE_STEP;
    }

    rotate<Shape>(object, bestAngle - currAngle);
}










