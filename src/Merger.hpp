#ifndef MERGER__H
#define MERGER__H

#include <algorithm>
#include <vector>

#include "Layout.hpp"

struct ShapeInfo {
    Point oldP1, oldP2;
    unsigned indexP1, indexP2, id;
    ShapeInfo(Point o1, Point o2, unsigned i1, unsigned i2, unsigned id) :
        oldP1(o1), oldP2(o2), indexP1(i1), indexP2(i2), id(id) {}
    unsigned getID() {
        return id;
    }
};

/**
 * Merger is meant to merge the data structures of Shapes. The merge() method receives a vector of group
 * of identifiers, each group begin assembled into a single shape.
 *
 * The user can later reverse the operation by calling reset(), which procedes to unmerge with the help
 * of data stored during the merge() process.
 *
 * As it was said, Merger is only meant to preserve structural integrity during a merge, not to
 * actually move the shapes close to each other (this is the work of Transformer)
 */
class Merger {
protected:
    // Contains parsed shapes;
    Layout& _shapes;
    // Contains information of old shapes before merging without their polygons (ids, reference points...)
    std::vector<ShapeInfo> _shapesInfos;
    // Contains former indices before merging for each new shape
    std::vector<std::vector<unsigned> > _shapesMerged;
public:
    //Constructor, receives a reference on our shape vector
    Merger(Layout& s) : _shapes(s) {
        for (Shape& s : _shapes) {
            //Store meta-data of every shape
            _shapesInfos.emplace_back(s.getOldP1(), s.getOldP2(), s.getIndexP1(), s.getIndexP2(),
                                      s.getID());
            //Initially, _shapesMerge = { {1}, {2}, ..., {n} }
            _shapesMerged.emplace_back(s.polyNumber(),
                                       s.getID()); //Constructor of vector initialized with size of _shapes and current index(with shapesMerged.size())
        }

        //Sort _shapesInfos by ID for future search operations
        sort(_shapesInfos.begin(), _shapesInfos.end(), [](const ShapeInfo & a,
        const ShapeInfo & b) {
            return a.id < b.id;
        });
    }
    void merge(std::vector<std::vector<unsigned> > shapesToMerge);
    void reset();
};

void mergeMultiP(MultiPolygon& A, const MultiPolygon& B);

/**
 * Creates a vector containing the IDs of the shapes in v (in the same order)
 */
template<typename VectorType>
std::vector<unsigned>* getIds(VectorType& v) {
    auto ret = new std::vector<unsigned>(v.size());

    for (unsigned i = 0 ; i < v.size() ; ++i)
        (*ret)[i] = v[i].getID();

    return ret;
}

#endif
