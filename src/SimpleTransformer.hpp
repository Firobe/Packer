#ifndef SIMPLETRANSFORMER__H
#define SIMPLETRANSFORMER__H

#include <iostream>
#include <string>

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
#include "Transformer.hpp"

#define ROTATE_STEP 30 //Step used when rotating
#define TRANSLATE_NB 15 //Number of translations test to make
#define STACKING_EPSILON 1. //Stacking precision : shapes stop when dist <= this
#define RENTABILITY 0.05 //Threshold of needed stacking efficiency

void applyTrans(Shape&, Shape&, double, double, unsigned, Box&, Box&, double,
                bool = false);

/**
 * Dirty bruteforce : tries arbritrary high number of rotations on two
 * objects, and with each configuration tries a fixed number of translation in
 * order to test different offsets between the shapes.
 *
 * Keeps the configuration when the intersection between the convex hulls of
 * the two shapes is the highest.
 */
template<typename CriteriaClass>
class SimpleTransformer : Transformer {
    private:
        CriteriaClass _criteriaInstance;
    public:
        SimpleTransformer(std::vector<Shape>& s) : Transformer(s) {}
        std::vector<std::vector<unsigned> > transform() {

            LOG(info) << "Now merging shapes";
			std::vector<std::vector<unsigned> > ret;
			std::vector<bool> mergedV(_shapes.size(), false);

            //We try to merge {0, 1}, {1, 2}, ..., {n - 1, n}
            for (unsigned i = 0 ; i < _shapes.size() - 1 ; ++i) {
                if (mergedV[i])
                    continue;

                LOG(debug) << "SimpleTransformer : i = " << i << std::endl;
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

                            double ratio = _criteriaInstance.criteria(shapeA, shapeB);
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
                            }//for offset
                        }//for beta
                    }//for alpha

                Box boxA, boxB, boxMerge;
                std::string textBase = "SimpleTransformer (" + std::to_string(i + 1) +
                                       "/" + std::to_string(_shapes.size() - 1) + ")";
                if (merged) {
                    LOG(info) << "!";
                    mergedV[i] = true;
                    mergedV[bestJ] = true;
                    ret.push_back({_shapes[i].getID(), _shapes[bestJ].getID()}); // _shapes update
                    applyTrans(_shapes[i], _shapes[bestJ], bestAlpha, bestBeta, bestOffset, boxA, boxB,
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

            LOG(info) << std::endl;
            return ret;
            }
    };


struct IntersectionCriteria {
    double criteria(const Shape&, const Shape&);
    };

struct BoxCriteria {
    double criteria(const Shape&, const Shape&);
    };
#endif
