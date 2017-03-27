#ifndef LAYOUT__H
#define LAYOUT__H

#include <vector>

#include "Shape.hpp"
#include "quadtree/QuadTree.hpp"

/**
 * @brief Layout class, meant to contain all the
 * shapes on the layout, and to be manipulated like a vector
 * It allows the use of Shapes or QuadTrees, depending on how it is
 * constructued
 */
class Layout {
    std::vector<Shape>* _s;
    std::vector<QuadTree>* _q;
    bool usingQuads;
    float precision;

    void generateQuadTrees(std::vector<Shape>* s);

public:
    Layout() : _s(nullptr), _q(nullptr) {}
    Layout(std::vector<Shape>* s) : _s(s), _q(nullptr), usingQuads(false) {
        //Shapes will be used
    }

    Layout(std::vector<Shape>* s, float precision) :
        //QuadTrees will be used
        _s(nullptr), _q(nullptr), usingQuads(true), precision(precision) {
        _q = new std::vector<QuadTree>;
        generateQuadTrees(s);
    }
    Layout(const Layout&);
    Layout& operator=(Layout& o) = delete;
    Layout& operator=(Layout&& o);
    ~Layout();
    void copy(const Layout& o);

    void replaceShapes(std::vector<Shape>* s);
    unsigned size() const;
    const Shape& operator[](unsigned n) const;
    Shape& operator[](unsigned n);
    void push_back(const Shape& s);
    void erase(unsigned n);
	bool useQuads() {
		return usingQuads;
	}

    class iterator : public std::iterator <
        std::random_access_iterator_tag,
        Shape,
        unsigned,
        Shape*,
        Shape& > {
        Layout& T;
        unsigned n;
    public:
        explicit iterator(Layout& T, unsigned n = 0) : T(T), n(n) {}
        bool operator==(iterator o) const {
            return n == o.n;
        }
        bool operator!=(iterator o) const {
            return n != o.n;
        }
        iterator& operator++() {
            ++n;
            return *this;
        }
        iterator operator++(int) {
            iterator b = *this;
            ++(*this);
            return b;
        }
        iterator& operator--() {
            --n;
            return *this;
        }
        iterator operator--(int) {
            iterator b = *this;
            --(*this);
            return b;
        }
        reference operator*() const {
            return T[n];
        }
        iterator& operator+=(unsigned i) {
            n += i;
            return *this;
        }
        iterator& operator-=(unsigned i) {
            n -= i;
            return *this;
        }
        iterator operator+(unsigned o) const {
            return iterator(T, n + o);
        }
        iterator operator-(unsigned o) const {
            return iterator(T, n - o);
        }
        unsigned operator-(iterator o) const {
            return n - o.n;
        }
        reference operator[](unsigned i) const {
            return *(*this + i);
        }
        bool operator<(iterator o) const {
            return n < o.n;
        }
        void operator=(const iterator& o) {
            n = o.n;
        }
    };
    iterator begin() {
        return iterator(*this);
    }
    iterator end() {
        return iterator(*this, size());
    }
};

#endif
