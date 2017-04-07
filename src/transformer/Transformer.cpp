#include "Transformer.hpp"

bool Transformer::generalStop = false;

Transformer::Transformer(Layout& s, const std::vector<Parameter>&) : _shapes(s) {
    //
}


Transformer::~Transformer() {
    //
}
