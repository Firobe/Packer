#include "SimpleTransformer.hpp"
#include "common.hpp"
#include "Log.hpp"


#include <boost/geometry/algorithms/buffer.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/strategies/cartesian/area_surveyor.hpp>
#include <boost/geometry/algorithms/covered_by.hpp>
#include <boost/geometry/strategies/agnostic/point_in_poly_winding.hpp>
#include <boost/geometry/strategies/agnostic/relate.hpp>
#include <boost/geometry/geometry.hpp>

std::vector<std::vector<int> > SimpleTransformer::transform() {
    std::vector<std::vector<int> > ret;
    for(int i = 0 ; i < static_cast<int>(_shapes.size()) ; i+=2) {
      LOG(info) << "CURRENTLY HUNGHING i= " << i << std::endl; 
      double bestAlpha, bestBeta, bestMid=0; // best rotations
      int bestUngh; // best translation (see PPAP for further information)
      float bestArea; // best area of merged couples of shapes
      if (i+1 < static_cast<int>(_shapes.size())) { // odd case
	ret.push_back({i,i+1}); // _shapes update
	// move shapes
	double alpha, beta;
	Shape shapeA, shapeB;
	Point ptA;
	Box boxA, boxB, boxMerge; //boxMerge est la bounding box resultant du merge
	shapeA = _shapes[i];
	shapeB = _shapes[i+1];
	bg::envelope(shapeA.getMultiP(),boxA);
	bg::envelope(shapeB.getMultiP(),boxB);
	bestArea = bg::area(boxA)+bg::area(boxB);
	bestAlpha = 0.;
	bestBeta = 0.;
	bestUngh = 0.;
	
	for(alpha = 0.; alpha < 360.; alpha += ROTATESTEP) {
	  LOG(info) << "alpha " << alpha << std::endl;
	  for(beta = 0.; beta < 360.; beta += ROTATESTEP) {
	    //LOG(info) << "beta " << beta << std::endl;
	    rotate<Shape>(shapeA,ROTATESTEP);
	    rotate<Shape>(shapeB,ROTATESTEP);
	    bg::centroid(shapeA.getMultiP(),ptA); // calcule le centre de gravite de shapeA (dans ptA)
	    translate<Shape>(shapeA,-ptA.x(),-ptA.y()); // ramène shapeA à l'origine
	    bg::envelope(shapeA.getMultiP(),boxA);
	    bg::envelope(shapeB.getMultiP(),boxB);
	    translate<Shape>(shapeB,-boxB.min_corner().x()+boxA.max_corner().x(),-boxB.min_corner().y()); // Coller les box au depart
	    for(unsigned ungh = 0; ungh < TRANSLATESTEPS ; ++ungh) {
	      double x1, x2, mid;
	      x1 = boxA.min_corner().x();
	      x2 = boxA.max_corner().x();
	      mid = (x2+x1)/2.;
	      // dichotomie pour emboiter propre
	      while((x2-x1) > EPSEMBOITEMENT) {
		mid = (x2+x1)/2.;
		translate<Shape>(shapeB,mid-x2,0.);
		if (bg::overlaps(shapeA.getMultiP(),shapeB.getMultiP())) {
		  x1 = mid;
		  translate<Shape>(shapeB,x2-mid,0.);
		}
		else {
		  x2 = mid;
		}
	      }
	      if (bg::overlaps(shapeA.getMultiP(),shapeB.getMultiP())) {
		LOG(error) <<"OULA OULAlalalalalalala"<<std::endl; 
	      }
	      MultiPolygon doubleP;
	      Box doubleBox;
	      doubleP.resize(2);
	      mergeMultiP(doubleP,shapeA.getMultiP(),shapeB.getMultiP()); 
	      bg::envelope(doubleP,doubleBox);
	      double doubleArea = bg::area(doubleBox);
	      if (doubleArea < bestArea) {
		bestArea = doubleArea;
		bestAlpha = alpha;
		bestBeta = beta;
		bestUngh = ungh;
		bestMid = mid;
	      }
	      translate(shapeB,boxA.max_corner().x()-mid+EPSEMBOITEMENT,(-boxB.max_corner().y()+boxB.min_corner().y())/((float) TRANSLATESTEPS)); // lel
	    }
	  } 
	}
	rotate<Shape>(_shapes[i],bestAlpha);
	rotate<Shape>(_shapes[i+1],bestBeta);
	bg::centroid(_shapes[i].getMultiP(),ptA);
	bg::envelope(_shapes[i].getMultiP(),boxA);
	bg::envelope(_shapes[i+1].getMultiP(),boxB);
	translate(_shapes[i],-ptA.x(),-ptA.y());
	translate(_shapes[i+1],-boxB.min_corner().x()+bestMid,-boxB.min_corner().y()-((boxB.min_corner().y()-boxB.max_corner().y())*((float) bestUngh))/((float) TRANSLATESTEPS));
	if (bg::overlaps(_shapes[i].getMultiP(),_shapes[i+1].getMultiP())) {
	  LOG(error) <<"OULA OULA OULA"<<std::endl; 
	}
      }
    }
    return ret;
}
