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

/**
 * HoleTransformer : basic algorithm to
 * fill holes in polygons with other shapes.
 * Will sort holes and shapes by decreasing area and
 * for each hole, try to insert each shape until one is found.
 */
vector<vector<unsigned> > HoleTransformer::transform() {
    LOG(info) << "Filling holes";
    vector<vector<unsigned> > ret; //List of shapes' index to merge
    vector<bool> mergedV(_shapes.size(), false);
    int h_area; //Hole area
    Point h_center(0., 0.), s_center(0., 0.);
    //Initial sort by global shape area, decreasing order
    sort(_shapes.begin(), _shapes.end(), [](const Shape & a, const Shape & b) {
        return bg::area(a.getMultiP()) > bg::area(b.getMultiP());
    });
    //Put the holes in decreasing order, with the id of the owner shape into a vector
    struct HoleOwner {
        HoleOwner(Ring* r, unsigned u) : hole(r), sID(u) {}
        Ring* hole;
        unsigned sID; //shapeID
    };
    vector<HoleOwner> holes;

    for (auto && shape : _shapes)
        for (auto && polygon : shape.getMultiP())
            for (auto && hole : polygon.inners())
                holes.push_back({&hole, shape.getID()});
    sort(holes.begin(), holes.end(), [](const HoleOwner & a, const HoleOwner & b) {
        return bg::area(*a.hole) > bg::area(*b.hole);
    });

    //Try to put shapes in every hole
    for (HoleOwner& hole : holes) {
        h_area = bg::area(*hole.hole);
        bg::centroid(*hole.hole, h_center);

        for (unsigned k = 0 ; k < _shapes.size() ;
                ++k) { //For each shape, starting with the largest
            if (bg::area(_shapes[k].getMultiP()) < h_area && !mergedV[k]) {
                bg::centroid(_shapes[k].getMultiP(), s_center);
                //Translate the shape's centroid on the hole's centroid
                translate(_shapes[k], h_center.x() - s_center.x(), h_center.y() - s_center.y());

                //If the shape fits, then merge
                if (!bg::overlaps(*hole.hole, _shapes[k].getMultiP())) {
                    mergedV[k] = true; //Will not use this shape again
                    /*Fill the ret vector accordingly
                    (find set containing hole.sID and add the shape into it,
                    create the set if not found)*/
                    bool found = false;

                    for (auto && e : ret) {
                        if (!e.empty() && e[0] == hole.sID) {
                            found = true;
                            e.push_back(_shapes[k].getID());
                            break;
                        }
                    }

                    if (!found)
                        ret.push_back({hole.sID, _shapes[k].getID()}); //Do NOT change the order
                    LOG(info) << "!";
                    break;
                }
            }
        }

        LOG(info) << ".";
    }

    LOG(info) << endl;
    return ret;
}


