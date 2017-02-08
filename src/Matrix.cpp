#include <iomanip>
#include <iostream>

#include "Matrix.hpp"

#define MAT_PRODUCT(x, y) lhs[x] * rhs[y]
#define MAT_CROSSPRODUCT(a, b, c, d) MAT_PRODUCT(a, b) + MAT_PRODUCT(c, d)

/**
 * + operator
 * Adds each coefficient and returns the result
 */
Matrix operator+(const Matrix& lhs, const Matrix& rhs) {
    Matrix ret = lhs;

    for (int i = 0 ; i < 6 ; ++i)
        ret[i] += rhs[i];

    return ret;
}

/**
 * * operator
 * Efficient matrix multiplication between lhs and rhs
 */
Matrix operator*(const Matrix& lhs, const Matrix& rhs) {
    return {
        MAT_CROSSPRODUCT(0, 0, 2, 1),
        MAT_CROSSPRODUCT(1, 0, 3, 1),
        MAT_CROSSPRODUCT(0, 2, 2, 3),
        MAT_CROSSPRODUCT(1, 2, 3, 3),
        MAT_CROSSPRODUCT(0, 4, 2, 5) + lhs[4],
        MAT_CROSSPRODUCT(1, 4, 3, 5) + lhs[5] };
}

/**
 * () operator
 * Applies a matrix to a vector (point)
 */
Point Matrix::operator()(const Point& p) const {
    return Point(
               _mat[0] * p.x() + _mat[2] * p.y() + _mat[4],
               _mat[1] * p.x() + _mat[3] * p.y() + _mat[5]);
}

/**
 * Stream operator
 * Enables to use a matrix this way :
 * cout << matrix;
 */
std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    os << "matrix(" << std::setprecision(9) << m[0];

    for (int i = 1 ; i < 6 ; ++i)
        os << ", " << m[i];

    os << ")";
    return os;
}

