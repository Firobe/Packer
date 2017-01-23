#ifndef SIMPLETRANSFORMER__H
#define SIMPLETRANSFORMER__H

#include "Transformer.hpp"

#define ROTATESTEP 1. // pas de rotation des shapes
#define TRANSLATESTEPS 8 // nombre de pas lors de la translation (emboitement)
#define EPSEMBOITEMENT 1. // precision d'emboitement a tant de pixels pres

class SimpleTransformer : Transformer{
public:
  std::vector<std::vector<int> > transform();
};

#endif
