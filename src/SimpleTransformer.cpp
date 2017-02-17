#include <iostream>

#include <boost/geometry/algorithms/intersects.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/algorithms/comparable_distance.hpp>
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
void applyTrans(Shape& a, Shape& b, double alpha, double beta, double offset, Box& boxA,
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
              offset);
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

vector<vector<unsigned> > SimpleTransformer::transform() {
    LOG(info) << "Merging shapes";
    vector<vector<unsigned> > ret;
    vector<bool> mergedV(_shapes.size(), false);
    //Initial shufle
    random_shuffle(_shapes.begin(), _shapes.end());

    //We try to merge {0, 1}, {1, 2}, ..., {n - 1, n}
    for (unsigned i = 0 ; i < _shapes.size() - 1 ; ++i) {
        if (mergedV[i])
            continue;

        LOG(debug) << "SimpleTransformer : i = " << i << endl;
        double bestAlpha = 0., bestBeta = 0., bestMid = 0; //Best rotations
        int bestOffset = 0.; //Best translation
        double bestArea = 0.; //Best area of merged couples of shapes
        unsigned j = i + 1;
        unsigned bestJ = j;
        //Move shapes
        bool merged = false; //Check if a merge occured
        #pragma omp parallel for schedule(dynamic, 1) collapse(3)

        for (int alpha = 0; alpha < 360; alpha += _rotateStep) { //Rotate first shape
            for (int beta = 0.; beta < 360 ; beta += _rotateStep) { //Rotate second shape
                for (unsigned offset = 0; offset < _translateNb ; ++offset) { //Trying different offsets
                    Box boxA, boxB;
                    Shape shapeA, shapeB;
                    shapeA = _shapes[i];
                    shapeB = _shapes[j];
                    applyTrans(shapeA, shapeB, alpha, beta, static_cast<double>(offset) / _translateNb, boxA,
                               boxB, 0, true);
                    double mid = getClose(shapeA, shapeB, boxA);
                    double ratio = _criteria(shapeA, shapeB, _rentability);
                    mergeMultiP(shapeA.getMultiP(), shapeB.getMultiP());
                    bg::envelope(shapeA.getMultiP(), boxA);
                    bool withinBin = boxA.max_corner().x() - boxA.min_corner().x() <
                                     Parser::getDims().x() and boxA.max_corner().y() -
                                     boxA.min_corner().y() < Parser::getDims().y();

                    //Computes intersection efficiency
                    //Store the best candidate
                    if (ratio > bestArea and withinBin) {
                        merged = true;
                        bestArea = ratio;
                        bestAlpha = alpha;
                        bestBeta = beta;
                        bestOffset = offset;
                        bestMid = mid;
                        bestJ = j;
                    }
                }
            }
        }

        string textBase = "SimpleTransformer (" + to_string(i + 1) +
                          "/" + to_string(_shapes.size() - 1) + ")";
        Box boxA, boxB;

        if (merged) {
            LOG(info) << "!";
            mergedV[i] = true;
            mergedV[bestJ] = true;
            ret.push_back({_shapes[i].getID(), _shapes[bestJ].getID()}); // _shapes update
            applyTrans(_shapes[i], _shapes[bestJ], bestAlpha, bestBeta,
                       static_cast<double>(bestOffset) / _translateNb, boxA, boxB,
                       bestMid);
            translate(_shapes[i], Parser::getDims().x() / 2., Parser::getDims().x() / 2.);
            translate(_shapes[bestJ], Parser::getDims().x() / 2., Parser::getDims().x() / 2.);
            Display::Update(_shapes[i].getID());
            Display::Update(_shapes[bestJ].getID());
            Display::Text(textBase + " : merged !");
        }
        else {
            LOG(info) << ".";
            Display::Text(textBase + " : failed to merge !");
            ret.push_back({_shapes[i].getID()});
        }
    }//for i

    LOG(info) << endl;
    return ret;
}

/**
 * Returns (if high enough) the area of the intersection between
 * the convex hulls of shapeA and shapeB
 */
double intersectionCriteria(const Shape& shapeA, const Shape& shapeB,
                            double rentability) {
    Polygon hullA, hullB;
    MultiPolygon inter;
    bg::convex_hull(shapeA.getMultiP(), hullA);
    bg::convex_hull(shapeB.getMultiP(), hullB);
    bg::intersection(hullA, hullB, inter);
    double ratio = bg::area(inter);
    return (ratio >= rentability * (bg::area(hullA) + bg::area(hullB))) * ratio;
}

/**
 * Returns (idem) the area of the bouding box around shapeA and shapeB
 */
double boxCriteria(const Shape& shapeA, const Shape& shapeB, double rentability) {
    Box bA, bB, inter;
    bg::envelope(shapeA.getMultiP(), bA);
    bg::envelope(shapeB.getMultiP(), bB);
    Point max_corner(max(bA.max_corner().x(), bB.max_corner().x()), max(bA.max_corner().y(),
                     bB.max_corner().y()));
    Point min_corner(min(bA.min_corner().x(), bB.min_corner().x()), min(bA.min_corner().y(),
                     bB.min_corner().y()));
    double ratio = (max_corner.x() - min_corner.x()) * (max_corner.y() - min_corner.y());
    return (ratio <= (1 - rentability) * (bg::area(bA) + bg::area(bB))) ? 1. / ratio : 0.;
}
