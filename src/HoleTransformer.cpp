#include <iostream>

#include <boost/geometry/algorithms/intersects.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/algorithms/comparable_distance.hpp>
#include <boost/geometry/algorithms/intersection.hpp>
#include <boost/geometry/algorithms/convex_hull.hpp>
#include <boost/geometry/algorithms/centroid.hpp>

#include "common.hpp"
#include "HoleTransformer.hpp"
#include "Log.hpp"
#include "Merger.hpp"
#include "Parser.hpp"
#include "Display.hpp"

using namespace std;

vector<vector<unsigned> > HoleTransformer::transform() {
    LOG(info) << "Filling holes";
    vector<vector<unsigned> > ret;
    vector<bool> mergedV(_shapes.size(), false);
    //Hole caracteristics 
    int h_area;
    Point h_centre, s_center;

    //Initial sort by global shape area
    sort(_shapes.begin(), _shapes.end(), [](const Shape & a, const Shape & b) {
	return bg::area(a.get_multiP()) < bg::area(b.get_multiP());
      });
    
    //Try to put small shapes in other shapes' holes
    for (unsigned i = 0 ; i < _shapes.size() ; ++i) {
      for (unsigned j = 0 ; j < _shapes[i].inners().size() ; ++j) {
	h_area = bg::area(_shapes[i].inners()[j]);
	h_center = bg::centroid(_shapes[i].inners()[j], h_center);
	for (unsigned k = _shapes.size() - 1 ; k > i ; --k) {
	  if (bg::area(_shapes[k]) > h_area && !mergeV[k]) { 
	    //Small enough to fill the hole and not already chosen to be merge
	    bg::centroid(_shapes[k]);
	    
    }

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

        for (int alpha = 0; alpha < 360; alpha += ROTATE_STEP) { //Rotate first shape
            for (int beta = 0.; beta < 360 ; beta += ROTATE_STEP) { //Rotate second shape
                for (unsigned offset = 0; offset < TRANSLATE_NB ; ++offset) { //Trying different offsets
                    Box boxA, boxB;
                    Shape shapeA, shapeB;
                    shapeA = _shapes[i];
                    shapeB = _shapes[j];
                    applyTrans(shapeA, shapeB, alpha, beta, offset, boxA, boxB, 0, true);
                    double mid = getClose(shapeA, shapeB, boxA);
                    double ratio = _criteria(shapeA, shapeB);
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

    LOG(info) << endl;
    return ret;
}

/**
 * Returns (if high enough) the area of the intersection between
 * the convex hulls of shapeA and shapeB
 */
double intersectionCriteria(const Shape& shapeA, const Shape& shapeB) {
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
double boxCriteria(const Shape& shapeA, const Shape& shapeB) {
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
