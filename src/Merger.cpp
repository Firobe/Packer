#include "Merger.hpp"
#include "common.hpp"

void Merger::merge(std::vector<std::vector<int> > shapesToMerge) {
    std::vector<int> shapesToPop;

    for (std::vector<int> bs : shapesToMerge) {
        for (unsigned i = 1 ; i < bs.size() ; ++i) {
            mergeMultiP(_shapes[bs[0]].getMultiP(), _shapes[bs[i]].getMultiP());
            shapesToPop.push_back(bs[i]);

            for (auto s : _shapesMerged[bs[i]]) {
                _shapesMerged[bs[0]].push_back(s);
            }
        }
    }

    std::sort(shapesToPop.begin(), shapesToPop.end(), [](const int& a, const int& b) {
        return a < b;
    }); // sorts shapesToPop increasingly

    for (unsigned i = 0 ; i < shapesToPop.size() ; ++i) {
        _shapes.erase(_shapes.begin() + shapesToPop[i] - i);
        _shapesMerged.erase(_shapesMerged.begin() + shapesToPop[i] - i);
    }
}

void Merger::reset() {
    std::vector<int> nbElem(_shapesInfos.size());

    for (auto& s : _shapesInfos) {
        s.getMultiP().resize(_shapesInfos.size());
    }

    for (unsigned i = 0; i < _shapes.size(); ++i) {
        for (unsigned j = 1; j < _shapesMerged[i].size(); ++j) {
            _shapesInfos[_shapesMerged[i][j]].getMultiP()[nbElem[_shapesMerged[i][j]]++]
                = _shapes[i].getMultiP()[j];
        }
    }

    for (unsigned i = 0; i < nbElem.size(); ++i) {
        _shapesInfos[i].getMultiP().resize(nbElem[i]);
    }

    _shapes.clear();

    for (auto& s : _shapesInfos) {
        _shapes.push_back(s);
    }
}
