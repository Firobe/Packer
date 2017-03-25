#include "Layout.hpp"
#include "Log.hpp"

using namespace std;

/**
 * @brief Generate a quadtree from each Shape in the parameter
 * @param s Pointer to a vector of raw Shapes
 */
void Layout::generateQuadTrees(std::vector<Shape>* s) {
    LOG(info) << "Generating quadtrees..." << endl;
    _q->clear();
    _q->reserve(s->size());

    for (auto && shape : *s)
        _q->emplace_back(shape, precision);

    delete s;
}

/**
 * @brief Replace the raw shapes by a new set of shapes
 * Will generate quadtrees again quadtrees are used
 * @param s New set of shapes
 */
void Layout::replaceShapes(std::vector<Shape>* s) {
    if (usingQuads)
        generateQuadTrees(s);
    else {
        if (_s != nullptr)
            delete _s;

        _s = s;
    }
}

/**
 * @brief Implementation of push_back with the classical behaviour
 * @param s Shape or QuadTree to push
 */
void Layout::push_back(const Shape& s) {
    if (usingQuads)
        _q->push_back(dynamic_cast<const QuadTree&>(s));
    else
        _s->push_back(s);
}

/**
 * @brief Delete value at index n from the layout
 * @param n
 */
void Layout::erase(unsigned n) {
    if (usingQuads)
        _q->erase(_q->begin() + n);
    else
        _s->erase(_s->begin() + n);
}

/**
 * @brief Return the number of shapes on the layout
 */
unsigned Layout::size() const {
    return usingQuads ? _q->size() : _s->size();
}

/**
 * @brief Returns the nth shape on the layout
 * @param n
 */
Shape& Layout::operator[](unsigned n) {
    return usingQuads ? (*_q)[n] : (*_s)[n];
}

/**
 * @brief const alternative of operator[]
 * @param n
 */
const Shape& Layout::operator[](unsigned n) const {
    return usingQuads ? (*_q)[n] : (*_s)[n];
}

/**
 * @brief Destructor, frees _s and _q if needed
 */
Layout::~Layout() {
    if (_s != nullptr)
        delete _s;

    _s = nullptr;

    if (_q != nullptr)
        delete _q;

    _q = nullptr;
}

/**
 * @brief Move assignment operator
 */
Layout& Layout::operator=(Layout&& o) {
    copy(o);
    o._q = nullptr;
    o._s = nullptr;
    return *this;
}

/**
 * @brief Default copy constructor
 */
void Layout::copy(const Layout& o) {
    _s = o._s;
    _q = o._q;
    usingQuads = o.usingQuads;
    precision = o.precision;
}

/**
 * @brief Copy constructor
 */
Layout::Layout(const Layout& o) {
    LOG(debug) << "Copying the whole layout !!" << endl;
    copy(o);

    if (usingQuads) {
        _s = nullptr;
        _q = new vector<QuadTree>;

        for (auto && c : *o._q)
            (*_q).push_back(c);
    }
    else {
        _s = new vector<Shape>;
        _q = nullptr;

        for (auto && c : *o._s)
            (*_s).push_back(c);
    }
}
