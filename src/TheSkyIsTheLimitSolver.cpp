#include <boost/geometry/algorithms/envelope.hpp>
#include <vector>

#include "TheSkyIsTheLimitSolver.hpp"
#include "types.hpp"

using namespace std;

/**
 * Defines a comparator for boxes
 */
struct {
    inline bool operator()(const Box& a, const Box& b) {
      return (a.max_corner().y()-a.min_corner().y()) < 
	(b.max_corner().y()-b.min_corner().y());
    }
} boxHeightLess;

/**
 * Defines a comparator for boxes
 */
struct {
  inline bool operator()(const Shape& a, const Shape& b) {
      Box ea;
      bg::envelope(a.getMultiP(),ea); 
      Box eb;
      bg::envelope(b.getMultiP(),eb);
      return boxHeightLess(ea,eb);
    }
} shapeHeightLess;

void TheSkyIsTheLimitSolver::solve() {
    vector<Box> boxes(_shapes.size());
    Point previous, reference;
    double currX, currY, offset;
    
    //Create the bounding boxes
    for (unsigned i = 0; i < _shapes.size(); i++) {
        bg::envelope(_shapes[i].getMultiP(), boxes[i]);
    }

    sort(_shapes.begin(), _shapes.end(), shapeHeightLess);
    sort(boxes.begin(), boxes.end(), boxHeightLess);

    for (unsigned i = 0; i < _shapes.size(); i++) {
      if (currX + boxes[i].max_corner().x() - boxes[i].min_corner().x() > _width){
	currY += offset;
	currX = 0;
	offset = boxes[i].max_corner().y() - boxes[i].min_corner().y();
	if (currY + offset > _height){
	  _binNumber++;
	  currY = _binNumber * _height;
	}
      }
      
      translate<Shape>(_shapes[i], currX - boxes[i].min_corner().x() , currY - boxes[i].min_corner().y());
      translate<Box>(boxes[i], currX - boxes[i].min_corner().x(), currY - boxes[i].min_corner().y());
      currY += boxes[i].max_corner().y() - boxes[i].min_corner().y();
      currX += boxes[i].max_corner().x() - boxes[i].min_corner().x();
    }

}


