#ifndef TRANSFORMER__H
#define TRANSFORMER__H

#include <algorithm>
#include <vector>

#include "Shape.hpp"

class Transformer {
protected:
    std::vector<Shape>& _shapes; // Contains parsed shapes;
public:
    Transformer(std::vector<Shape>& s) : _shapes(s) {}
    virtual std::vector<std::vector<int> > transform() = 0;
};

#endif
