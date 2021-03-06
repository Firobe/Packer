#include <iostream>

#include <boost/geometry/algorithms/distance.hpp>

#include "Layout.hpp"
#include "Log.hpp"
#include "Parser.hpp"
#include "solver/Solver.hpp"

using namespace std;

/**
 * @brief Updates the vector of reference
 */
void Layout::generateRefs() {
    ShapeVec::clear();

    if (usingQuads)
        for (QuadTree& s : *_q)
            ShapeVec::emplace_back(s);
    else
        for (Shape& s : *_s)
            ShapeVec::emplace_back(s);
}

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
    if (usingQuads) {
        /*
        vector<Matrix> mats;
        mats.reserve(s->size());

        for(auto&& sh : *s){
        	mats.emplace_back(sh.getTransMatrix());
        	sh.restore();
        }*/
        generateQuadTrees(s);
        /*
        for(unsigned i = 0 ; i < mats.size() ; ++i)
        	(*s)[i].applyMatrix(mats[i]);
        	*/
    }
    else {
        if (_s != nullptr)
            delete _s;

        _s = s;
    }

    generateRefs();
}

/**
 * @brief Implementation of push_back with the classical behaviour
 * @param s Shape or QuadTree to push
 */
void Layout::push_back(Shape& s) {
    if (usingQuads) {
        QuadTree& t = dynamic_cast<QuadTree&>(s);
        _q->push_back(t);
        ShapeVec::push_back(t);
    }
    else {
        _s->push_back(std::move(s));
        ShapeVec::push_back(s);
    }
}

/**
 * @brief Delete value at index n from the layout
 * @param n
 */
void Layout::erase(unsigned n) {
    ShapeVec::erase(ShapeVec::begin() + n);
    /*
    if (usingQuads)
        _q->erase(_q->begin() + n);
    else
        _s->erase(_s->begin() + n);
    	*/
}

/**
 * @brief Returns the nth shape on the layout
 * @param n
 */
Shape& Layout::operator[](unsigned n) {
    return ShapeVec::operator[](n);
}

/**
 * @brief const alternative of operator[]
 * @param n
 */
const Shape& Layout::operator[](unsigned n) const {
    return ShapeVec::operator[](n);
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

    for (Shape* s : _copies)
        delete s;
}

/**
 * @brief Move assignment operator
 */
Layout& Layout::operator=(Layout&& o) {
    ShapeVec::operator=(o);
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
Layout::Layout(const Layout& o) : ShapeVec(o) {
    LOG(info) << "Copying the whole layout !!" << endl;
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

        for (Shape& c : *o._s) {
            Shape s(c);
            (*_s).push_back(move(s));
        }
    }

    generateRefs();
}

/**
 * @brief Create a new Shape/QuadTree
 * and return a reference on it
 */
Shape& Layout::newCopy() {
    Shape* s = nullptr;
    #pragma omp critical
    {
        if (usingQuads)
            s = new QuadTree();
        else s = new Shape();

        _copies.push_back(s);
    }
    return *s;
}

/**
 * @brief Makes a deep copy of tc into c
 * @param tc Shape to copy
 * @param c Recipient of copy
 */
void Layout::copyShape(Shape& c, const Shape& tc) {
    if (usingQuads)
        c.operator = (dynamic_cast<const QuadTree&>(tc));
    else c.operator = (tc);
}

void Layout::forceApply() {
    if (usingQuads) {
        for (Shape& s : *this) {
            Matrix m = s.getTransMatrix();
            s.restore(false);
            s.applyMatrix(m, false, false);
        }
    }
}

/**
 * @brief Creates a Solution from the current state
 * (generates every transformation Matrix)
 * @param s Where the solution will be generated
 */
void Layout::genSolution(Solution& s) const {
    s.mats.clear();
    s.mats.reserve(size());

    for (Shape& sh : *this)
        s.mats.push_back(sh.getTransMatrix());

    s.quality = quality();
}

/**
 * @brief Apply a solution to the current state
 * (apply every transformation matrix)
 * @param s The solution to apply
 */
void Layout::applySolution(Solution& s) {
    for (unsigned i = 0 ; i < size() ; ++i) {
        operator[](i).restore();
        operator[](i).applyMatrix(s.mats[i]);
    }
}

/**
 * @brief Evaluates the quality of a solution
 * Currently sums the squared distance of each shape
 * to the origin
 */
double Layout::quality() const {
    double sum = 0;
    Box e;

    for (unsigned i = 0 ; i < size() ; ++i) {
        Point c = operator[](i).centroid();
        operator[](i).envelope(e);
        //int binNb = e.min_corner().y() / (Parser::getDims().y() * SPACE_COEF);
        Point orig(0, 0);//Parser::getDims().y() * SPACE_COEF * binNb);
        double d = bg::distance(orig, c);
        sum += d * d;
    }

    return sum;
}
