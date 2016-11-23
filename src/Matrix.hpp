#ifndef MATRIX__H
#define MATRIX__H

#include <boost/array.hpp>
#include <array>
#include <stack>

#include "types.hpp"

class Matrix {
private:
	std::array<double, 6> _mat;
public:
	Matrix(double a, double b, double c, double d,
			double e, double f) : _mat({a, b, c, d, e, f}) {}
	Matrix(boost::array<double, 6> m) :
		_mat({m[0], m[1], m[2], m[3], m[4], m[5]}) {}
	double& operator[](std::size_t idx){
		return _mat[idx];
	}
	const double& operator[](std::size_t idx) const{
		return _mat[idx];
	}
	Point operator()(const Point&);
	bool operator==(const Matrix& r){
		return _mat == r._mat;
	}
	bool operator!=(const Matrix& r){
		return !operator==(r);
	}
};

std::ostream& operator<<(std::ostream& os, const Matrix& m);
Matrix operator+(Matrix lhs, const Matrix& rhs);
Matrix operator*(Matrix lhs, const Matrix& rhs);

class MatStack : public std::stack<Matrix>{
public:
	MatStack(){
		emplace(1, 0, 0, 1, 0, 0);
	}
	void append(const Matrix& m){
		push(top() * m);
	}
};

#endif
