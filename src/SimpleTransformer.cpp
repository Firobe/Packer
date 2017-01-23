#include "SimpleTransformer.hpp"
#include "common.hpp"

SimpleTransformer::transform() {
    std::vector<std::vector<int> > ret;
    for(int i = 0 ; i < _shapes.size() ; i+=2) {
      double bestAlpha, bestBeta, bestMid; // best rotations
      int bestUngh; // best translation (see PPAP for further information)
      float bestArea; // best area of merged couples of shapes
      if (i+1 < _shapes.size()) { // odd case
	ret.emplace_back({i,i+1}); // _shapes update
	// move shapes
	double alpha, beta;
	Shape shapeA, shapeB;
	Point ptA;
	Box boxA, boxB, boxMerge; //boxMerge est la bounding box resultant du merge
	shapeA = _shapes[i];
	shapeB = _shapes[i+1];
	envelope(shapeA.getMultiP(),boxA);
	envelope(shapeB.getMultiP(),boxB);
	bestArea = area(boxA)+area(boxB);
	bestAlpha = 0.;
	bestBeta = 0.;
	bestUngh = 0.;
	
	for(alpha = 0., alpha < 360., alpha += STEP) {
	  for(beta = 0., beta < 360., beta += STEP) {

	    rotate<Shape>(shapeA,alpha);
	    rotate<Shape>(shapeB,beta);
	    centroid(shapeA,ptA); // calcule le centre de gravite de shapeA (dans ptA)
	    translate<Shape>(shapeA,-ptA.x(),-ptA.y()); // ramène shapeA à l'origine
	    envelope(shapeA.getMultiP(),boxA);
	    envelope(shapeB.getMultiP(),boxB);
	    translate<Shape>(shapeB,-boxB.minCorner().x()+boxA.maxCorner().x(),-boxB.minCorner().y()); // Coller les box au depart
	    for(int ungh = 0, ungh < TRANSLATESTEPS ; ++ungh) {
	      float x1, x2, mid;
	      x1 = boxA.minCorner().x();
	      x2 = boxA.maxCorner().x();
	      // dichotomie pour emboiter propre
	      while((x2-x1) > EPSEMBOITEMENT) {
		mid = (x2-x1)/2.;
		translate<Shape>(shapeB,mid-x2,0.);
		if (overlaps(shapeA,shapeB)) {
		  x1 = mid;
		  translate<Shape>(shapeB,x2-mid,0.);
		}
		else {
		  x2 = mid;
		}
	      }
	      multiP doubleP;
	      Box doubleBox;
	      doubleP.resize(2);
	      fusionMultiPs(doubleP,shapeA.getMultiP(),shapeB.getMultiP()); //NIY
	      envelope(doubleP,doubleBox);
	      double doubleArea = area(doubleBox);
	      if (doubleArea < bestArea) {
		bestArea = doubleArea;
		bestAlpha = alpha;
		bestBeta = beta;
		bestUngh = ungh;
		bestMid = mid;
	      }
	      translate(shapeB,boxA.maxCorner().x()-mid+EPSEMBOITEMENT,(-boxB.maxCorner().y()+boxB.minCorner.y())/((float) TRANSLATESTEPS)); // lel
	    }
	  } 
	}
	rotate<Shape>(_shapes[i],bestAlpha);
	rotate<Shape>(_shapes[i+1],bestBeta);
	centroid(_shapes[i],ptA);
	envelope(_shapes[i].getMultiP(),boxA);
	envelope(_shapes[i+1].getMultiP(),boxB);
	translate(_shapes[i],-ptA.x(),-ptA.y());
	translate(_shapes[i+1],-boxB.minCorner().x()+bestMid,-boxB.minCorner.y()-((boxB.minCorner.y()-boxB.maxCorner.y())*((float) bestUngh))/((float) TRANSLATESTEPS));
      }
    }
    return ret;
}
