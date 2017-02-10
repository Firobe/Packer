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

#define ROTATE_STEP 30 //Step used when rotating
#define TRANSLATE_NB 15 //Number of translations test to make
#define STACKING_EPSILON 1. //Stacking precision : shapes stop when dist <= this
#define RENTABILITY 0.05 //Threshold of needed stacking efficiency

using namespace std;

/**
 * See SimpleTransformer documentation
 */
vector<vector<unsigned> > SimpleTransformer::transform() {
    LOG(info) << "Now merging shapes";
    vector<vector<unsigned> > ret;
    vector<bool> mergedV(_shapes.size(), false);

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
        #pragma omp parallel for schedule(dynamic, 1)

        for (int alpha = 0; alpha < 360; alpha += ROTATE_STEP) { //Rotate first shape
            Shape shapeA, shapeB;
            Box boxA, boxB, boxMerge;

            for (int beta = 0.; beta < 360 ; beta += ROTATE_STEP) { //Rotate second shape
                for (unsigned offset = 0; offset < TRANSLATE_NB ; ++offset) { //Trying different offsets
                    shapeA = _shapes[i];
                    shapeB = _shapes[j];
                    applyTrans(shapeA, shapeB, alpha, beta, offset, boxA, boxB, 0, true);
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

                    //Computes intersection efficiency
                    Polygon hullA, hullB;
                    MultiPolygon inter;
                    bg::convex_hull(shapeA.getMultiP(), hullA);
                    bg::convex_hull(shapeB.getMultiP(), hullB);
                    bg::intersection(hullA, hullB, inter);
                    double ratio = bg::area(inter);
                    mergeMultiP(shapeA.getMultiP(), shapeB.getMultiP());
                    bg::envelope(shapeA.getMultiP(), boxA);
                    bool withinBin = boxA.max_corner().x() - boxA.min_corner().x() < Parser::getDims().x()
                                     and boxA.max_corner().y() - boxA.min_corner().y() < Parser::getDims().y();

                    //Store the best candidate
                    if (ratio > bestArea and ratio >= RENTABILITY * (bg::area(hullA) + bg::area(hullB))
                            and withinBin) {
                        merged = true;
                        bestArea = ratio;
                        bestAlpha = alpha;
                        bestBeta = beta;
                        bestOffset = offset;
                        bestMid = mid;
                        bestJ = j;
                    }
                }//for offset
            }//for beta
        }//for alpha

        Box boxA, boxB, boxMerge;

        if (merged) {
            LOG(info) << "!";
            mergedV[i] = true;
            mergedV[bestJ] = true;
#ifdef ENABLE_DISPLAY
            /*
            Display::Update(_shapes[i].getID());
            Display::Update(_shapes[bestJ].getID());
            */
            Display::Reset();
#endif
            LOG(debug) << "===========================> MERGED <=====================" << endl;
            ret.push_back({_shapes[i].getID(), _shapes[bestJ].getID()}); // _shapes update
            applyTrans(_shapes[i], _shapes[bestJ], bestAlpha, bestBeta, bestOffset, boxA, boxB,
                       bestMid);
        }
        else {
            LOG(info) << ".";
            ret.push_back({_shapes[i].getID()});
        }
    }//for i

    LOG(info) << endl;
    return ret;
}

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
