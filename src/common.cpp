#include "common.hpp"

void mergeMultiP(MultiPolygon& A, const MultiPolygon& B) {
    for (auto& b : B) {
        A.push_back(b);
    }
}
