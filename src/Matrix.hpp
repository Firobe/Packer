#ifndef MATRIX__H
#define MATRIX__H

#include <boost/array.hpp>
#include <array>
#include <stack>

#include "types.hpp"

/**
 * A SVG Matrix. In the form of :
 * a c e
 * b d f
 * 0 0 1
 *
 * Overloads [], (), <<, + and * operators.
 * To apply a matrix to a vector : mat(vect);
 */
class Matrix {
private:
    std::array<double, 6> _mat;
public:
	//Constructors :
	//Matrix(a, b, c, d, e, f)
    Matrix(double a, double b, double c, double d,
           double e, double f) : _mat( {
        a, b, c, d, e, f
    }) {}

	//Matrix(T) when T supports [] operator
    template<typename T>
    Matrix(T m) :
        _mat( {
        m[0], m[1], m[2], m[3], m[4], m[5]
    }) {}
    double& operator[](std::size_t idx) {
        return _mat[idx];
    }
    const double& operator[](std::size_t idx) const {
        return _mat[idx];
    }
    Point operator()(const Point&) const;
    bool operator==(const Matrix& r) const {
        return _mat == r._mat;
    }
    bool operator!=(const Matrix& r) const {
        return !operator==(r);
    }
};

std::ostream& operator<<(std::ostream& os, const Matrix& m);
Matrix operator+(Matrix lhs, const Matrix& rhs);
Matrix operator*(const Matrix& lhs, const Matrix& rhs);

/**
 * A Matrix stack. Mainly used in the parser.
 * Inherits standard stack.
 * Usage :
 * push, pop, top
 * append : push a matrix multiplied by the top of the stack
 */
class MatStack : public std::stack<Matrix> {
public:
    MatStack() {
        emplace(1, 0, 0, 1, 0, 0);
    }
    void append(const Matrix& m) {
        push(top() * m);
    }
};

#endif
