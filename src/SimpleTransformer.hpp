#ifndef SIMPLETRANSFORMER__H
#define SIMPLETRANSFORMER__H

#include "Transformer.hpp"

#define ROTATESTEP 20. // pas de rotation des shapes
#define TRANSLATESTEPS 15 // nombre de pas lors de la translation (emboitement)
#define EPSEMBOITEMENT 1. // precision d'emboitement a tant de pixels pres

class SimpleTransformer : Transformer {
public:
    SimpleTransformer(std::vector<Shape>& s) : Transformer(s) {}
    std::vector<std::vector<int> > transform();
};

void bloubla(Shape& a, Shape& b, double alpha, double beta, unsigned offset, Box& boxA,
             Box& boxB, double mid, bool midmid);
#endif
