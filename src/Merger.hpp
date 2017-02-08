#ifndef MERGER__H
#define MERGER__H

#include <algorithm>
#include <vector>

#include "Shape.hpp"

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
        std::vector<Shape>& _shapes;
		// Contains information of old shapes before merging without their polygons (ids, reference points...)
        std::vector<Shape> _shapesInfos;
		// Contains former indices before merging for each new shape
        std::vector<std::vector<unsigned> > _shapesMerged;
    public:
		//Constructor, receives a reference on our shape vector
        Merger(std::vector<Shape>& s) : _shapes(s) {
            for (Shape s : _shapes) {
				//Store meta-data of every shape
                _shapesInfos.emplace_back(s.getOldP1(), s.getOldP2(), s.getIndexP1(), s.getIndexP2(),
                                          s.getID());
				//Initially, _shapesMerge = { {1}, {2}, ..., {n} }
                _shapesMerged.emplace_back(s.getMultiP().size(),
                                           s.getID()); //Constructor of vector initialized with size of _shapes and current index(with shapesMerged.size())
                }

			//Sort _shapesInfos by ID for future search operations
            sort(_shapesInfos.begin(), _shapesInfos.end(), [](const Shape & a, const Shape & b) {
                return a.getID() < b.getID();
                });
            }
        void merge(std::vector<std::vector<unsigned> > shapesToMerge);
        void reset();
    };

void mergeMultiP(MultiPolygon& A, const MultiPolygon& B);
std::vector<unsigned>* getIds(std::vector<Shape>& v);

#endif
