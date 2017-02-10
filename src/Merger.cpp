#include "Merger.hpp"
#include "common.hpp"
#include "Display.hpp"

using namespace std;

/**
 * Merges the data structures of each group received in shapesToMerge
 * shapesToMerge[i] = vector of identifiers to merge into a new shape
 */
void Merger::merge(vector<vector<unsigned> > shapesToMerge) {
    sort(_shapes.begin(), _shapes.end(), [](const Shape & a, const Shape & b) {
        return a.getID() < b.getID();
    });
    auto ids = getIds(_shapes);

    for (vector<unsigned> idTab : shapesToMerge) {
        for (unsigned i = 1 ; i < idTab.size() ; ++i) {
            unsigned sod = binaryFind(*ids,
                                      idTab[0]);//Index in _shapes of the first identifier to merge
            unsigned sid = binaryFind(*ids, idTab[i]);//Idem for i-th shape
            mergeMultiP(_shapes[sod].getMultiP(), _shapes[sid].getMultiP()); //Merge polygons
            _shapesMerged[sod] = _shapesMerged[sod] + _shapesMerged[sid]; //Concatenation
            _shapes.erase(_shapes.begin() + sid);
            _shapesMerged.erase(_shapesMerged.begin() + sid);
            ids->erase(ids->begin() + sid);
        }
    }

    delete ids;

#ifndef ENABLE_DISPLAY
	Display::Reset();
#endif
}

/**
 * Reverse operation of merge(), unmerges previously merged shapes
 * with the help of information stored by merge()
 *
 * reset() is supposed to be called after merge()
 */
void Merger::reset() {
    sort(_shapes.begin(), _shapes.end(), [](const Shape & a, const Shape & b) {
        return a.getID() < b.getID();
    });
    auto ids = getIds(_shapesInfos);
    auto ids2 = getIds(_shapes);

    for (auto& s : _shapesInfos)
        s.getMultiP().reserve(_shapesInfos.size());

    for (unsigned i = 0; i < _shapes.size(); ++i) {
        unsigned sod = binaryFind(*ids2, _shapes[i].getID());

        for (unsigned j = 0; j < _shapesMerged[sod].size(); ++j) {
            unsigned sid = binaryFind(*ids, _shapesMerged[i][j]);
            _shapesInfos[sid].getMultiP().push_back(_shapes[i].getMultiP()[j]);
        }
    }

    _shapes.clear();

    for (auto& s : _shapesInfos)
        _shapes.push_back(s);

    delete ids;
    delete ids2;

#ifndef ENABLE_DISPLAY
	Display::Reset();
#endif
}

/**
 * Stores all of B's polygons into A
 */
void mergeMultiP(MultiPolygon& A, const MultiPolygon& B) {
    for (auto& b : B)
        A.push_back(b);
}

/**
 * Creates a vector containing the IDs of the shapes in v (in the same order)
 */
vector<unsigned>* getIds(vector<Shape>& v) {
    auto ret = new vector<unsigned>(v.size());

    for (unsigned i = 0 ; i < v.size() ; ++i)
        (*ret)[i] = v[i].getID();

    return ret;
}
