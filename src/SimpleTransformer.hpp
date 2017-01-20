#ifndef SIMPLETRANSFORMER__H
#define SIMPLETRANSFORMER__H

#include "Transformer.hpp"

#define STEP 1.

class SimpleTransformer : Transformer{
public:
  std::vector<std::vector<int> > transform() {
    std::vector<std::vector<int> > ret;
    for(int i = 0 ; i < _shapes.size() ; i+=2) {
      if (i+1 < _shapes.size()) { // odd case
	ret.emplace_back({i,i+1}); // _shapes update
	// move shapes
	double alpha, beta;
	for(alpha = 0., alpha < 360., alpha += STEP) {
for(beta = 0., beta < 360., beta += STEP) {
//RAPPROCHER
}

	}
	
      }
    }  
  }
};

#endif
