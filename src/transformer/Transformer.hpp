#ifndef TRANSFORMER__H
#define TRANSFORMER__H

#include <vector>

#include "Layout.hpp"
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
    Layout& _shapes; // Contains parsed shapes
public:
	static bool generalStop;
    Transformer(Layout& s, const std::vector<Parameter>&);
    virtual std::vector<std::vector<unsigned> > transform() = 0;
    virtual ~Transformer();
};

#endif
