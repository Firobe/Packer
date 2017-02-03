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

using namespace std;

vector<vector<int> > SimpleTransformer::transform() {
    vector<vector<int> > ret;

    for (int i = 0 ; i < static_cast<int>(_shapes.size()) ; i += 2) {
        LOG(info) << "CURRENTLY HoffsetING i= " << i << endl;
        double bestAlpha, bestBeta, bestMid = 0; //Best rotations
        int bestOffset; //Best translation (see PPAP for further information)
        double bestArea; //Best area of merged couples of shapes

        if (i + 1 < static_cast<int>(_shapes.size())) { // odd case
            //Move shapes
            bool merged = false;
            //boxMerge est la bounding box resultant du merge
            bestArea = 0.;
            bestAlpha = 0.;
            bestBeta = 0.;
            bestOffset = 0.;

#pragma omp parallel for schedule(dynamic, 1)
            for (int alpha = 0; alpha < 360; alpha += ROTATESTEP) {
				Shape shapeA, shapeB;
				Box boxA, boxB, boxMerge;
                LOG(info) << "alpha " << alpha << endl;

                for (double beta = 0.; beta < 360.; beta += ROTATESTEP) {
                    for (unsigned offset = 0; offset < TRANSLATESTEPS ; ++offset) {
                        shapeA = _shapes[i];
                        shapeB = _shapes[i + 1];
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

                        if (ratio > bestArea) {
                            merged = true;
                            bestArea = ratio;
                            bestAlpha = alpha;
                            bestBeta = beta;
                            bestOffset = offset;
                            bestMid = mid;
                        }
                    }
                }
            }

			Box boxA, boxB, boxMerge;
            if (merged) {
                cerr << "bestAlpha " << bestAlpha << " bestBeta " << bestBeta << " bestOffset " <<
                     bestOffset << " bestMid " << bestMid << endl;
                ret.push_back({i, i + 1}); // _shapes update
                bloubla(_shapes[i], _shapes[i + 1], bestAlpha, bestBeta, bestOffset, boxA, boxB, bestMid);
            }
            else {
                ret.push_back({i});
                ret.push_back({i + 1});
            }

            if (bg::overlaps(_shapes[i].getMultiP(), _shapes[i + 1].getMultiP())) {
                LOG(error) << "OULA OULA OULA" << endl;
            }
        }
    }

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
              -boxB.min_corner().x() + (midmid ? boxA.max_corner().x() : mid) + (1 - midmid) * EPSEMBOITEMENT,
              -boxA.max_corner().y() -
              length *
              (static_cast<double>(offset) / static_cast<double>(TRANSLATESTEPS)));
}
