#ifndef HOLETRANSFORMER__H
#define HOLETRANSFORMER__H

#include <iostream>
#include <algorithm>
#include <string>

#include "Transformer.hpp"
#include "CloseEnough.hpp"

/**
 * Dirty bruteforce : tries arbritrary high number of rotations on two
 * objects, and with each configuration tries a fixed number of translation in
 * order to test different offsets between the shapes.
 *
 * Keeps the configuration when the intersection between the convex hulls of
 * the two shapes is the highest.
 */
class HoleTransformer : public Transformer {
public:
    HoleTransformer(std::vector<Shape>& s, std::vector<Parameter> params) :
        Transformer(s, params) {}
    std::vector<std::vector<unsigned> > transform();
};

#endif
