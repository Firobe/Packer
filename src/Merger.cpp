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
            _shapes[sod].mergeWith(_shapes[sid]); //Merge polygons
            _shapesMerged[sod] = _shapesMerged[sod] + _shapesMerged[sid]; //Concatenation
            _shapes.erase(sid);
            _shapesMerged.erase(_shapesMerged.begin() + sid);
            ids->erase(ids->begin() + sid);
        }
    }

    delete ids;
    Display::Reset();
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
    vector<Shape>* after = new vector<Shape>;

    for (auto& i : _shapesInfos) {
        after->emplace_back(i.oldP1, i.oldP2, i.indexP1, i.indexP2, i.id);
        after->back().reserve(_shapesInfos.size());
    }

    for (unsigned i = 0; i < _shapes.size(); ++i) {
        unsigned sod = binaryFind(*ids2, _shapes[i].getID());

        for (unsigned j = 0; j < _shapesMerged[sod].size(); ++j) {
            unsigned sid = binaryFind(*ids, _shapesMerged[i][j]);
            (*after)[sid].addNthPolygon(_shapes[i], j);
        }
    }

    _shapes.replaceShapes(after);
    delete ids;
    delete ids2;
    Display::Reset();
}


