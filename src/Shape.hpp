#ifndef SHAPE__H
#define SHAPE__H

#include <string>
#include <array>

#include "types.hpp"

/**
 * Shape class. Designed to be
 * carried around by the different
 * modules of the program.
 *
 * Inherits a boost MultiP and stores two
 * points at initialization so it can
 * compute its transformation matrix later.
 *
 * Also stores it's ID in the original SVG file.
 */
class Shape {
private:
    MultiPolygon _multiP;
    Point _oldP1, _oldP2;
    std::string _id;

    void fillShape(std::vector<Ring>&);  //Initializes a Shape from an array of rings
public:
    Shape(std::vector<Ring>& r, std::string id) : _id(id) {
        fillShape(r);
    }

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
};

template<>
void rotate<Shape>(Shape& object, double angle);

template<>
void translate<Shape>(Shape& object, double x, double y);

#endif
