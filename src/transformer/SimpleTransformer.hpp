#ifndef SIMPLETRANSFORMER__H
#define SIMPLETRANSFORMER__H

#include <iostream>
#include <algorithm>
#include <string>

#include "Transformer.hpp"
#include "CloseEnough.hpp"

#define ROTATE_STEP 30 //Step used when rotating
#define TRANSLATE_NB 15 //Number of translations test to make
#define STACKING_EPSILON 1. //Stacking precision : shapes stop when dist <= this
#define RENTABILITY 0.05 //Threshold of needed stacking efficiency

using SimpleTransformerCriteria =
    std::function<double(const Shape&, const Shape&, double)> ;

double intersectionCriteria(const Shape&, const Shape&, double);
double boxCriteria(const Shape&, const Shape&, double);

/**
 * Dirty bruteforce : tries arbritrary high number of rotations on two
 * objects, and with each configuration tries a fixed number of translation in
 * order to test different offsets between the shapes.
 *
 * Keeps the configuration when the intersection between the convex hulls of
 * the two shapes is the highest.
 */
class SimpleTransformer : public Transformer {
private:
    SimpleTransformerCriteria _criteria;
    double _rentability;
    int _rotateStep,  _translateNb;
public:
    SimpleTransformer(Layout& s, const std::vector<Parameter>& params) :
        Transformer(s, params) {
        std::string criteria;

        if (!getParameter(params, "criteria", criteria))
            throw std::runtime_error("Please specify a correct criteria for the transformer");

        if (criteria == "intersection")
            _criteria = intersectionCriteria;
        else if (criteria == "box")
            _criteria = boxCriteria;

        if (!getParameter(params, "rotate_step", _rotateStep))
            _rotateStep = ROTATE_STEP;

        if (!getParameter(params, "translate_nb", _translateNb))
            _translateNb = TRANSLATE_NB;

        if (!getParameter(params, "rentability", _rentability))
            _rentability = RENTABILITY;
    }
    std::vector<std::vector<unsigned> > transform();
};

void applyTrans(Shape&, Shape&, double, double, double, Box&, Box&, double,
                bool = false);
double getClose(Shape& a, Shape& b, Box&);

#endif
