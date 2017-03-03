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
    bool merge;
    int h_area; //Hole area
    Point h_centre, s_center;

    //Initial sort by global shape area, decreasing order
    sort(_shapes.begin(), _shapes.end(), [](const Shape & a, const Shape & b) {
	return bg::area(a.get_multiP()) < bg::area(b.get_multiP());
      });
    
    //Try to put small shapes in other shapes' holes
    for (unsigned i = 0 ; i < _shapes.size() ; ++i) { //for each shape, starting with the biggest
        merge = false;
      for (unsigned j = 0 ; j < _shapes[i].inners().size() ; ++j) { //for each hole (if there is any)
	       h_area = bg::area(_shapes[i].inners()[j]);
	       h_center = bg::centroid(_shapes[i].inners()[j], h_center);
	       for (unsigned k = _shapes.size() - 1 ; k > i ; --k) { //for each shape, starting with the smallest
	           if (bg::area(_shapes[k]) < h_area && !mergeV[k]) { 
	               bg::centroid(_shapes[k], s_center);
                   translate(_shapes[k], h_center.x()-s_center.x(), h_center.y()-s_center.y() );
                   if (!bg::intersects(_shapes[i].getMultiP(), _shapes[k].getMultiP())) {
                        mergedV[k] = true;
                        merge = true;
                        ret.push_back({_shapes[i].getID(), _shapes[k].getID()})
                   }
                }
            }
        }
        if (merge == false){
            ret.push_back({_shapes[i].getID()});
        }
    }

    LOG(info) << endl;
    return ret;
}

