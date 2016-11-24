#include "Matrix.hpp"

#define MAT_TERM(x, y) lhs[x] * rhs[y]
#define MAT_SUPERTERM(a, b, c, d) MAT_TERM(a, b) + MAT_TERM(c, d)

Matrix operator+(Matrix lhs, const Matrix& rhs) {
    for (int i = 0 ; i < 6 ; i++) {
        lhs[i] += rhs[i];
    }

    return lhs;
}

Matrix operator*(Matrix lhs, const Matrix& rhs) {
    Matrix ret(0, 0, 0, 0, 0, 0);
    ret[0] = MAT_SUPERTERM(0, 0, 2, 1);
    ret[1] = MAT_SUPERTERM(1, 0, 3, 1);
    ret[2] = MAT_SUPERTERM(0, 2, 2, 3);
    ret[3] = MAT_SUPERTERM(1, 2, 3, 3);
    ret[4] = MAT_SUPERTERM(0, 4, 2, 5) + lhs[4];
    ret[5] = MAT_SUPERTERM(1, 4, 3, 5) + lhs[5];
    return ret;
}

Point Matrix::operator()(const Point& p) {
    return Point(
               _mat[0] * p.x() + _mat[2] * p.y() + _mat[4],
               _mat[1] * p.x() + _mat[3] * p.y() + _mat[5]);
}

std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    os << "matrix(" << m[0];

    for (int i = 1 ; i < 6 ; i++) {
        os << ", " << m[i];
    }

    os << ")";
    return os;
}

