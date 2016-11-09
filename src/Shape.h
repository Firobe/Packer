#ifndef SHAPE__H
#define SHAPE__H

#include <string>
#include <boost/geometry.hpp>
#include <array>
#include "types.h"

class Shape : public Ring {
private:
    std::string _tag;
    Point _oldP1, _oldP2;
    int _indexP1, indexP2;

    void fillShape(vector<Point>&);
public:
    Shape(std::string s, vector<Point>& p) : _tag(s) {
        fillShape(p);
    }
    std::string getTag() const {
        return _tag;
    }
    std::array<double, 6> getTransMatrix() const;
};

#endif
