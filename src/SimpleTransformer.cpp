#include <iostream>

#include <boost/geometry/algorithms/intersects.hpp>
#include <boost/geometry/algorithms/intersection.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/algorithms/convex_hull.hpp>
#include <boost/geometry/algorithms/centroid.hpp>
#include <boost/geometry/algorithms/comparable_distance.hpp>

#include "common.hpp"
#include "SimpleTransformer.hpp"
#include "Log.hpp"
#include "Merger.hpp"
#include "Parser.hpp"
#include "Display.hpp"


using namespace std;

/**
 * Transormations on shapes a and b :
 * rotations of respectively alpha and beta
 * a is translated to the origin
 * b is translated on the same height as a
 * b is translated by mid on the x-axis (if start, else by the width of a)
 * b is translated relatively to offset on the y-axis
 */
void applyTrans(Shape& a, Shape& b, double alpha, double beta, unsigned offset, Box& boxA,
                Box& boxB, double mid, bool start) {
    Point ptA(0, 0);
    rotate(a, alpha);
    rotate(b, beta);
    bg::centroid(a.getMultiP(), ptA);
    translate(a, -ptA.x(), -ptA.y());
    bg::envelope(a.getMultiP(), boxA);
    bg::envelope(b.getMultiP(), boxB);
    double length = abs(boxA.max_corner().y() - boxA.min_corner().y()) +
                    abs(boxB.max_corner().y() - boxB.min_corner().y());
    translate(b,
              -boxB.min_corner().x() + (start ? boxA.max_corner().x() : mid) +
              (1 - start) * STACKING_EPSILON,
              -boxA.max_corner().y() -
              length *
              (static_cast<double>(offset) / static_cast<double>(TRANSLATE_NB)));
    }

double IntersectionCriteria::criteria(const Shape& shapeA, const Shape& shapeB) {
    Polygon hullA, hullB;
    MultiPolygon inter;
    bg::convex_hull(shapeA.getMultiP(), hullA);
    bg::convex_hull(shapeB.getMultiP(), hullB);
    bg::intersection(hullA, hullB, inter);
    double ratio = bg::area(inter);

    return (ratio >= RENTABILITY * (bg::area(hullA) + bg::area(hullB))) ? ratio : 0.;
    }

double BoxCriteria::criteria(const Shape& shapeA, const Shape& shapeB) {
	Box bA, bB, inter;
	bg::envelope(shapeA.getMultiP(), bA);
	bg::envelope(shapeB.getMultiP(), bB);
	bg::intersection(bA, bB, inter);
    double ratio = bg::area(inter);

    return (ratio >= RENTABILITY * (bg::area(bA) + bg::area(bB))) ? ratio : 0.;
    }
