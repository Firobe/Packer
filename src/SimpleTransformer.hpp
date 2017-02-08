#ifndef SIMPLETRANSFORMER__H
#define SIMPLETRANSFORMER__H

#include "Transformer.hpp"

/**
 * Dirty bruteforce : tries arbritrary high number of rotations on two
 * objects, and with each configuration tries a fixed number of translation in
 * order to test different offsets between the shapes.
 *
 * Keeps the configuration when the intersection between the convex hulls of
 * the two shapes is the highest.
 */
class SimpleTransformer : Transformer {
public:
    SimpleTransformer(std::vector<Shape>& s) : Transformer(s) {}
    std::vector<std::vector<unsigned> > transform();
};

void applyTrans(Shape&, Shape&, double, double, unsigned, Box&, Box&, double,
                bool = false);
#endif
