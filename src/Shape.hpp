#ifndef SHAPE__H
#define SHAPE__H

#include <iostream> //need to remove after
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
    friend class QuadTree; //QuadTree can access protected members from every Shape object
    friend class CustomShape;
protected:
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
    Shape(const MultiPolygon& p, unsigned id) : _multiP(p), _id(id) {
        setOld();
    }
    Shape(Point P1, Point P2, unsigned i1, unsigned i2, unsigned id) : _oldP1(P1),
        _oldP2(P2), _indexP1(i1), _indexP2(i2), _id(id) {}
    virtual Shape& operator=(const Shape& s) {
		std::cerr << "shape assignement called" << std::endl;
        _multiP = s._multiP;
        _oldP1 = s._oldP1;
        _oldP2 = s._oldP2;
        _indexP1 = s._indexP1;
        _indexP2 = s._indexP2;
        _id = s._id;
        _out = s._out;
        return *this;
    }
    virtual ~Shape() {}

    //Getters - Setters
    unsigned getID() const;
    Point getOldP1() const;
    Point getOldP2() const;
    unsigned getIndexP1() const;
    unsigned getIndexP2() const;
    const std::string& getIdentifier() const;
    const std::string& getOut() const;
    void appendOut(const std::string& s);

    //MultiP
    void addNthPolygon(const Shape& s, unsigned n);
    void reserve(int nbPoly);
    unsigned polyNumber() const;
    const Polygon& getNthPoly(unsigned i) const;

    //Algorithms
    virtual void mergeWith(const Shape&);
    virtual void rotate(double degrees);
    virtual void translate(double Tx, double Ty);
    virtual void envelope(Box&) const;
    virtual Point centroid() const;
	virtual int area() const;
    virtual bool intersectsWith(const Shape&) const;
    virtual bool intersectsWith(const Ring& s) const;
    virtual void convexHull(Polygon&) const;
    virtual void bufferize(double buffer);
    virtual std::array<double, 6> getTransMatrix() const;

    //Debug
    std::string debugOutputSVG() const;
    friend std::string debugOutputSVG(const std::vector<Shape>&);
};

void rotateToBestAngle(Shape& object);

#endif
