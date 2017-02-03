#include "common.hpp"

void mergeMultiP(MultiPolygon &ret, const MultiPolygon &A, const MultiPolygon &B){
  for(auto &a : A) ret.push_back(a);
  for(auto &b : B) ret.push_back(b);
}
