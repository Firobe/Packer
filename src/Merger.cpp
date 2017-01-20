#include "Merger.hpp"


void Merger::merge(std::vector<std::vector<int> > shapesToMerge) {
    std::vector<int> shapesToPop;
    for(std::vector<int> bs : shapesToMerge) {
      for(int i = 1 ; i < bs.size() ; ++i) {
        fusionMultiPs(_shapes[bs[0]],_shapes[bs[i]]); // NIY
	shapesToPop.push_back(bs[i])
	fusionTab(_shapesMerged[bs[0]],_shapesMerged[bs[i]]) // NIY
      }
    }
    std::sort(shapesToPop.begin(),shapesToPop.end(),[](const int& a, const int& b){return a<b}); // sorts shapesToPop increasingly
    for(int i = 0 ; i < shapesToPop.size() ; ++i) {
      _shapes.erase(_shapes.begin()+shapesToPop[i]-i);
      _shapesMerged.erase(_shapesMerged.begin()+shapesToPop[i]-i);
    }
  }
