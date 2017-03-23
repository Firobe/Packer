#ifndef HOLETRANSFORMER__H
#define HOLETRANSFORMER__H

#include <iostream>
#include <algorithm>
#include <string>

#include "Transformer.hpp"
#include "CloseEnough.hpp"

/**
 * This transformer intends to fill every hole of every shape with another
 * existing shape.
 * For now, shapes are only translated and undergo no rotation to fill holes.
 * This is a first fit algorithm, we choose the first shape that goes into the
 * hole without collision. The test is made on smallest shapes (area criteria) first.
 *
 */
class HoleTransformer : public Transformer {
public:
    HoleTransformer(std::vector<Shape>& s, const std::vector<Parameter>& params) :
        Transformer(s, params) {}
    std::vector<std::vector<unsigned> > transform();
};

#endif
