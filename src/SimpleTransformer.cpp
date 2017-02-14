#include <iostream>

#include <boost/geometry/algorithms/intersection.hpp>
#include <boost/geometry/algorithms/convex_hull.hpp>
#include <boost/geometry/algorithms/centroid.hpp>

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


/**
 * Operates a dichotomy by moving shapeB so that
 * its as close to shapeA as possible (STACKING_EPSILON)
 * without intersecting each other
 * (uses precomputed boxA)
 */
double getClose(Shape& shapeA, Shape& shapeB, Box& boxA) {
    //Dichotomy to find closest non-intersecting position (by translating on the x-axis)
    double x1, x2, mid;
    x1 = boxA.min_corner().x();
    x2 = boxA.max_corner().x();
    mid = (x2 + x1) / 2.;

    while ((x2 - x1) > STACKING_EPSILON) {
        mid = (x2 + x1) / 2.;
        translate<Shape>(shapeB, mid - x2, 0.);

        if (bg::intersects(shapeA.getMultiP(), shapeB.getMultiP())) {
            x1 = mid;
            translate<Shape>(shapeB, x2 - mid, 0.);
        }
        else
            x2 = mid;
    }

    return mid;
}

/**
 * Returns (if high enough) the area of the intersection between
 * the convex hulls of shapeA and shapeB
 */
double IntersectionCriteria::criteria(const Shape& shapeA, const Shape& shapeB) {
    Polygon hullA, hullB;
    MultiPolygon inter;
    bg::convex_hull(shapeA.getMultiP(), hullA);
    bg::convex_hull(shapeB.getMultiP(), hullB);
    bg::intersection(hullA, hullB, inter);
    double ratio = bg::area(inter);
    return (ratio >= RENTABILITY * (bg::area(hullA) + bg::area(hullB))) * ratio;
}

/**
 * Returns (idem) the area of the bouding box around shapeA and shapeB
 */
double BoxCriteria::criteria(const Shape& shapeA, const Shape& shapeB) {
    Box bA, bB, inter;
    bg::envelope(shapeA.getMultiP(), bA);
    bg::envelope(shapeB.getMultiP(), bB);
    Point max_corner(max(bA.max_corner().x(), bB.max_corner().x()), max(bA.max_corner().y(),
                     bB.max_corner().y()));
    Point min_corner(min(bA.min_corner().x(), bB.min_corner().x()), min(bA.min_corner().y(),
                     bB.min_corner().y()));
    double ratio = (max_corner.x() - min_corner.x()) * (max_corner.y() - min_corner.y());
    return (ratio <= (1 - RENTABILITY) * (bg::area(bA) + bg::area(bB))) ? 1. / ratio : 0.;
}
