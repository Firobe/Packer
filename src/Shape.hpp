#ifndef SHAPE__H
#define SHAPE__H

#include <string>
#include <array>

#include "common.hpp"

#define BUFFER_POINTS_PER_CIRCLE 10

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
    std::string _id;
    std::string _out;

    void fillShape(std::vector<Ring>&); //Initializes a Shape from an array of rings
    void setOld();
public:
    Shape() {}
    Shape(std::vector<Ring>& r, std::string id) : _id(id), _out() {
        fillShape(r);
    }

    Shape(Point P1, Point P2, unsigned i1, unsigned i2, std::string id) : _oldP1(P1),
        _oldP2(P2), _indexP1(i1), _indexP2(i2), _id(id) {}

    std::string getID() const {
        return _id;
    }
    std::array<double, 6> getTransMatrix() const;
    const MultiPolygon& getMultiP() const {
        return _multiP;
    }
    MultiPolygon& getMultiP() {
        return _multiP;
    }
    void bufferize(int buffer);
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



};

template <>
void rotate <Shape> (Shape& object, double angle);

template <>
void translate <Shape> (Shape& object, double x, double y);

#endif
