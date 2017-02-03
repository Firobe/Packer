#ifndef SIMPLETRANSFORMER__H
#define SIMPLETRANSFORMER__H

#include "Transformer.hpp"

#define ROTATESTEP 45. // pas de rotation des shapes
#define TRANSLATESTEPS 8 // nombre de pas lors de la translation (emboitement)
#define EPSEMBOITEMENT 1. // precision d'emboitement a tant de pixels pres

class SimpleTransformer : Transformer{
public:
  SimpleTransformer(std::vector<Shape>& s) : Transformer(s) {}
  std::vector<std::vector<int> > transform();
};

#endif
