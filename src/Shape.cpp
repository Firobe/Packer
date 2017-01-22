#include <algorithm>
#include <cmath>
#include <iostream>

#include <boost/geometry/algorithms/buffer.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/strategies/cartesian/area_surveyor.hpp>
#include <boost/geometry/algorithms/covered_by.hpp>
#include <boost/geometry/strategies/agnostic/point_in_poly_winding.hpp>
#include <boost/geometry/strategies/agnostic/relate.hpp>

#include "Shape.hpp"
#include "Log.hpp"

using namespace std;

/**
 * Defines a comparator for Rings
 */
inline bool ringLess(const Ring& a, const Ring& b) {
    return bg::area(a) < bg::area(b);
}

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
 * (of <buffer> px) around each of them.
 * _oldP1, P2, indexP1, P2 are also recalculated according to the new multiPolygon
 */
void Shape::bufferize(int buffer) {
    // Declare strategies
    const int points_per_circle = BUFFER_POINTS_PER_CIRCLE;
    bg::strategy::buffer::distance_symmetric<double> distance_strategy(buffer);
    bg::strategy::buffer::join_round join_strategy(points_per_circle);
    bg::strategy::buffer::end_round end_strategy(points_per_circle);
    bg::strategy::buffer::point_circle circle_strategy(points_per_circle);
    bg::strategy::buffer::side_straight side_strategy;
    MultiPolygon result;
    // Create the buffer of a multi polygon
    bg::buffer(_multiP, result,
               distance_strategy, side_strategy,
               join_strategy, end_strategy, circle_strategy);
    _multiP = result;
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


// Rotation pour boxer à l'aire minimale
void rotateToBestAngle (Shape& object) {
	const double ANGLE_MAX = 90.0;
	const double ANGLE_STEP = 0.2;
	
	
	double bestAngle, currAngle;
	double bestArea, currArea;
	Box currBox;
	
	bestAngle = 0.0;
	bg::envelope(object.getMultiP(), currBox);
	bestArea = (currBox.max_corner().x()-currBox.min_corner().x())*
			   (currBox.max_corner().y()-currBox.min_corner().y());
	
	translate<Shape>(object, -currBox.min_corner().x(), -currBox.min_corner().y());	
	
	currAngle = 0.0;
	while(currAngle <= ANGLE_MAX) {
		rotate<Shape>(object, ANGLE_STEP);
		
		bg::envelope(object.getMultiP(), currBox);
		currArea = (currBox.max_corner().x()-currBox.min_corner().x())*
				   (currBox.max_corner().y()-currBox.min_corner().y());
		
		if(currArea < bestArea) {
			bestArea = currArea;
			bestAngle = currAngle;
		}
		
		currAngle += ANGLE_STEP;
	}
	
	rotate<Shape>(object, bestAngle-currAngle);
}










