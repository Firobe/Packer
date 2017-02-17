#ifndef TRANSFORMER__H
#define TRANSFORMER__H

#include <vector>

#include "Shape.hpp"
#include "common.hpp"

/**
 * Abstract class for transformers.
 * Transformers are meant to apply arbritrary transformations on shape.
 *
 * As opposed to Solvers, transformers are not final treatments, and do not check position
 * on bin. There were created in order to merge shapes, and this is their main goal.
 */
class Transformer {
protected:
    std::vector<Shape>& _shapes; // Contains parsed shapes
public:
    Transformer(std::vector<Shape>& s, std::vector<Parameter>) : _shapes(s) {}
    virtual std::vector<std::vector<unsigned> > transform() = 0;
};

#endif
