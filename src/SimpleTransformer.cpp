#include <iostream>

#include <boost/geometry/algorithms/buffer.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/strategies/cartesian/area_surveyor.hpp>
#include <boost/geometry/algorithms/covered_by.hpp>
#include <boost/geometry/strategies/agnostic/point_in_poly_winding.hpp>
#include <boost/geometry/strategies/agnostic/relate.hpp>
#include <boost/geometry/algorithms/convex_hull.hpp>
#include <boost/geometry/geometry.hpp>

#include "common.hpp"
#include "SimpleTransformer.hpp"
#include "Log.hpp"

//#define OPTIMAL
#define RENTABILITY 0.05

using namespace std;

vector<vector<unsigned> > SimpleTransformer::transform() {
    vector<vector<unsigned> > ret;
    vector<bool> mergedV(_shapes.size(), false);

    for (unsigned i = 0 ; i < _shapes.size() ; ++i) {
        if (mergedV[i]) {
            continue;
        }

        LOG(info) << "CURRENTLY HoffsetING i= " << i << endl;
        double bestAlpha, bestBeta, bestMid = 0; //Best rotations
        int bestOffset; //Best translation (see PPAP for further information)
        double bestArea; //Best area of merged couples of shapes
        unsigned j = i + 1;
    	unsigned bestJ = j;
        bestArea = 0.;
        bestAlpha = 0.;
        bestBeta = 0.;
        bestOffset = 0.;
#ifdef OPTIMAL

        for (; j < _shapes.size() ; ++j)
            if (!mergedV[j])
#endif //OPTIMAL
                if (j < _shapes.size()) { // odd case
                    //Move shapes
                    bool merged = false;
                    //boxMerge est la bounding box resultant du merge
                    #pragma omp parallel for schedule(dynamic, 1)

                    for (int alpha = 0; alpha < 360; alpha += ROTATESTEP) {
                        Shape shapeA, shapeB;
                        Box boxA, boxB, boxMerge;
                        //LOG(info) << "alpha " << alpha << endl;

                        for (double beta = 0.; beta < 360.; beta += ROTATESTEP) {
                            for (unsigned offset = 0; offset < TRANSLATESTEPS ; ++offset) {
                                shapeA = _shapes[i];
                                shapeB = _shapes[j];
                                bloubla(shapeA, shapeB, alpha, beta, offset, boxA, boxB, 0, true);
                                double x1, x2, mid;
                                x1 = boxA.min_corner().x();
                                x2 = boxA.max_corner().x();
                                mid = (x2 + x1) / 2.;

                                // dichotomie pour emboiter propre
                                while ((x2 - x1) > EPSEMBOITEMENT) {
                                    mid = (x2 + x1) / 2.;
                                    translate<Shape>(shapeB, mid - x2, 0.);

                                    if (bg::overlaps(shapeA.getMultiP(), shapeB.getMultiP())) {
                                        x1 = mid;
                                        translate<Shape>(shapeB, x2 - mid, 0.);
                                    }
                                    else {
                                        x2 = mid;
                                    }
                                }

                                Polygon hullA, hullB;
                                MultiPolygon inter;
                                bg::convex_hull(shapeA.getMultiP(), hullA);
                                bg::convex_hull(shapeB.getMultiP(), hullB);
                                bg::intersection(hullA, hullB, inter);
                                double ratio = bg::area(inter);

                                if (ratio > bestArea && ratio >= RENTABILITY * (bg::area(hullA) + bg::area(hullB))) {
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
                        mergedV[i] = true;
                        mergedV[bestJ] = true;
                        LOG(info) << "===========================> UNGH <=====================" << endl;
                        ret.push_back({_shapes[i].getID(), _shapes[bestJ].getID()}); // _shapes update
                        bloubla(_shapes[i], _shapes[bestJ], bestAlpha, bestBeta, bestOffset, boxA, boxB, bestMid);
                    }
                    else {
                        ret.push_back({_shapes[i].getID()});
                    }
                } //if
    }//for i

    return ret;
}

void bloubla(Shape& a, Shape& b, double alpha, double beta, unsigned offset, Box& boxA,
             Box& boxB, double mid, bool midmid) {
    Point ptA;
    rotate(a, alpha);
    rotate(b, beta);
    bg::centroid(a.getMultiP(), ptA);
    translate(a, -ptA.x(), -ptA.y());
    bg::envelope(a.getMultiP(), boxA);
    bg::envelope(b.getMultiP(), boxB);
    double length = abs(boxA.max_corner().y() - boxA.min_corner().y()) +
                    abs(boxB.max_corner().y() - boxB.min_corner().y());
    translate(b,
              -boxB.min_corner().x() + (midmid ? boxA.max_corner().x() : mid) +
              (1 - midmid) * EPSEMBOITEMENT,
              -boxA.max_corner().y() -
              length *
              (static_cast<double>(offset) / static_cast<double>(TRANSLATESTEPS)));
}
