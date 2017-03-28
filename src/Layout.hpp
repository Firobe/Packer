#ifndef LAYOUT__H
#define LAYOUT__H

#include <vector>

#include "Shape.hpp"
#include "quadtree/QuadTree.hpp"

using ShapeVec = std::vector<std::reference_wrapper<Shape>>;

/**
 * @brief Layout class, meant to contain all the
 * shapes on the layout, and to be manipulated like a vector
 * It allows the use of Shapes or QuadTrees, depending on how it is
 * constructued
 * It is iterable because of the ShapeVec inheritance
 */
class Layout : public ShapeVec {
    std::vector<Shape>* _s;
    std::vector<QuadTree>* _q;
    std::vector<Shape*> _copies;
    bool usingQuads;
    float precision;

    void generateQuadTrees(std::vector<Shape>* s);
    void copy(const Layout& o);
public:
    Layout() : _s(nullptr), _q(nullptr) {}
    Layout(std::vector<Shape>* s) : ShapeVec(), _s(s), _q(nullptr), usingQuads(false) {
        //Shapes will be used
        generateRefs();
    }

    Layout(std::vector<Shape>* s, float precision) : ShapeVec(),
        _s(nullptr), _q(nullptr), usingQuads(true), precision(precision) {
        //QuadTrees will be used
        _q = new std::vector<QuadTree>;
        generateQuadTrees(s);
        generateRefs();
    }
    Layout(const Layout&);
    Layout& operator=(Layout& o) = delete;
    Layout& operator=(Layout&& o);
    ~Layout();

    void generateRefs();
    Shape& newCopy();
    void copyShape(Shape&, const Shape&);
    void replaceShapes(std::vector<Shape>* s);
    const Shape& operator[](unsigned n) const;
    Shape& operator[](unsigned n);
    void push_back(Shape& s);
    void erase(unsigned n);
};

#endif
