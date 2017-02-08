#include "Merger.hpp"
#include "common.hpp"

using namespace std;

void Merger::merge(vector<vector<int> > shapesToMerge) {
    vector<int> shapesToPop;
    sort(_shapes.begin(), _shapes.end(), [](const Shape & a, const Shape & b) {
        return a.getID() < b.getID();
    });
    vector<int> ids(_shapes.size());

    for (unsigned i = 0 ; i < ids.size() ; ++i) {
        ids[i] = _shapes[i].getID();
    }

    for (vector<int> idTab : shapesToMerge) {
        unsigned sod = binaryFind(ids, idTab[0]);

        for (unsigned i = 1 ; i < idTab.size() ; ++i) {
            unsigned sid = binaryFind(ids, idTab[i]);
            mergeMultiP(_shapes[sod].getMultiP(), _shapes[sid].getMultiP());
            shapesToPop.push_back(sid);

            for (auto s : _shapesMerged[sid]) {
                _shapesMerged[sod].push_back(s);
            }
        }
    }

    sort(shapesToPop.begin(), shapesToPop.end(), [](const int& a, const int& b) {
        return a < b;
    }); // sorts shapesToPop increasingly

    for (unsigned i = 0 ; i < shapesToPop.size() ; ++i) {
        _shapes.erase(_shapes.begin() + shapesToPop[i] - i);
        _shapesMerged.erase(_shapesMerged.begin() + shapesToPop[i] - i);
    }

    for (auto& i : _shapesMerged) {
        cerr << endl;

        for (auto& j : i) {
            cerr << j << " ; ";
        }
    }

    cerr << endl;
}

void Merger::reset() {
    vector<int> nbElem(_shapesInfos.size());
    sort(_shapesInfos.begin(), _shapesInfos.end(), [](const Shape & a, const Shape & b) {
        return a.getID() < b.getID();
    });
    sort(_shapes.begin(), _shapes.end(), [](const Shape & a, const Shape & b) {
        return a.getID() < b.getID();
    });
    vector<int> ids(_shapesInfos.size());
    vector<unsigned int> ids2(_shapes.size());

    for (unsigned i = 0 ; i < ids.size() ; ++i) {
        ids[i] = _shapesInfos[i].getID();
    }

    for (unsigned i = 0 ; i < ids2.size() ; ++i) {
        ids2[i] = _shapes[i].getID();
    }

    for (auto& s : _shapesInfos) {
        s.getMultiP().resize(_shapesInfos.size());
    }

    for (unsigned i = 0; i < _shapes.size(); ++i) {
        unsigned sod = binaryFind(ids2, _shapes[i].getID());

        for (unsigned j = 0; j < _shapesMerged[sod].size(); ++j) {
            unsigned sid = binaryFind(ids, _shapesMerged[i][j]);
            _shapesInfos[sid].getMultiP()[nbElem[sid]++]
                = _shapes[i].getMultiP()[j];
        }
    }

    for (unsigned i = 0; i < nbElem.size(); ++i) {
        _shapesInfos[i].getMultiP().resize(nbElem[i]);
    }

    _shapes.clear();
    cerr << "Shapesinfo size " << _shapesInfos.size() << endl;

    for (auto& s : _shapesInfos) {
        _shapes.push_back(s);
    }
}

void mergeMultiP(MultiPolygon& A, const MultiPolygon& B) {
    for (auto& b : B) {
        A.push_back(b);
    }
}
