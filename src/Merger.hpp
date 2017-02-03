#ifndef MERGER__H
#define MERGER__H

#include <string>
#include <algorithm>
#include <list>
#include <vector>

#include <boost/geometry/algorithms/envelope.hpp>

#include "Shape.hpp"

class Merger {
protected:
    std::vector<Shape>& _shapes; // Contains parsed shapes;
    std::vector<Shape>
    _shapesInfos; // Contains information of old shapes before merging without their polygons (ids, reference points...)
    std::vector<std::vector<int> >
    _shapesMerged; // Contains former indices before merging for each new shape
public:
    Merger(std::vector<Shape>& s) : _shapes(s) {
        for (Shape s : _shapes) {
            _shapesInfos.emplace_back(s.getOldP1(), s.getOldP2(), s.getIndexP1(), s.getIndexP2(),
                                      s.getID());
            _shapesMerged.emplace_back(s.getMultiP().size(),
                                       _shapesMerged.size()); //Constructor of vector initialized with size of _shapes and current index(with shapesMerged.size())
        }
    }
    void merge(std::vector<std::vector<int> > shapesToMerge);
    void reset();
};

void mergeMultiP(MultiPolygon& A, const MultiPolygon& B);

#endif
