#include "Merger.hpp"
#include "common.hpp"


void Merger::merge(std::vector<std::vector<int> > shapesToMerge) {
    std::vector<int> shapesToPop;
    for(std::vector<int> bs : shapesToMerge) {
      for(int i = 1 ; i < bs.size() ; ++i) {
        mergeMultiP(_shapes[bs[0]].getMultiP(),_shapes[bs[0]].getMultiP(),_shapes[bs[i]].getMultiP()); 
	shapesToPop.push_back(bs[i]);
	for(auto s : _shapesMerged[bs[i]])
	  _shapesMerged[bs[0]].push_back(s); 
      }
    }
    std::sort(shapesToPop.begin(),shapesToPop.end(),[](const int& a, const int& b){return a<b;}); // sorts shapesToPop increasingly
    for(int i = 0 ; i < shapesToPop.size() ; ++i) {
      _shapes.erase(_shapes.begin()+shapesToPop[i]-i);
      _shapesMerged.erase(_shapesMerged.begin()+shapesToPop[i]-i);
    }
  }
