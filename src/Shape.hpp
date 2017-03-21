#ifndef SHAPE__H
#define SHAPE__H

#include <string>
#include <array>

#include "common.hpp"

/**
 * Shape class. Designed to be
 * carried around by the different
 * modules of the program.
 *
 * contain a boost MultiP and stores two
 * points at initialization so it can
 * compute its transformation matrix later.
 *
 * Also stores it's ID in the original SVG file.
 */
class Shape {
private:
    MultiPolygon _multiP;
    Point _oldP1, _oldP2;
    unsigned _indexP1, _indexP2;
    unsigned _id; //index in Parser::id()
    std::string _out;

    void fillShape(std::vector<Ring>&); //Initializes a Shape from an array of rings
    void setOld();
public:
    Shape() {}
    Shape(std::vector<Ring>& r, unsigned id) : _id(id), _out() {
        fillShape(r);
    }

    Shape(Point P1, Point P2, unsigned i1, unsigned i2, unsigned id) : _oldP1(P1),
        _oldP2(P2), _indexP1(i1), _indexP2(i2), _id(id) {}

    unsigned getID() const {
        return _id;
    }
    void addNthPolygon(const Shape& s, unsigned n) {
        _multiP.push_back(s._multiP[n]);
    }
    void mergeWith(const Shape&);
    void reserve(int nbPoly) {
        _multiP.reserve(nbPoly);
    }
    //Informations on Shape
    unsigned polyNumber() const {
        return _multiP.size();
    }
    Polygon getNthPoly(unsigned i) const {
        return _multiP[i];
    }
    //Adaptation of boost algorithms
    void rotate(double degrees);
    void translate(double Tx, double Ty);
    void envelope(Box&) const;
    Point centroid() const;
    int area() const;
    bool intersectsWith(const Shape&) const;
    bool intersectsWith(const Ring& s) const;
    void convexHull(Polygon&) const;

    const std::string& getIdentifier() const;

    std::array<double, 6> getTransMatrix() const;
    void bufferize(double buffer);
    void appendOut(const std::string& s) {
        _out += s;
    }
    const std::string& getOut() {
        return _out;
    }

    Point getOldP1() {
        return _oldP1;
    }
    Point getOldP2() {
        return _oldP2;
    }
    unsigned getIndexP1() {
        return _indexP1;
    }
    unsigned getIndexP2() {
        return _indexP2;
    }

    std::string debugOutputSVG() const;
    friend std::string debugOutputSVG(const std::vector<Shape>&);
};

void rotateToBestAngle(Shape& object);

#endif
