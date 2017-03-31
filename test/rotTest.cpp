#include <string>
#include <stdexcept>

#include <boost/geometry/geometry.hpp>
#include "Layout.hpp"
#include "commonTest.hpp"
#include "Parser.hpp"
#include "Matrix.hpp"

//sugina
using namespace std;

int main() {
	/*
	Ring r {{100, 100}, {100, 200}, {200, 200}, {200, 100}, {100, 100}};
	bg::correct(r);
	MultiPolygon mp;
	bg::convert(r, mp);
	vector<Shape> v;
	v.emplace_back(mp, 0);
	Shape s(mp, 0);
	Layout l(&v, 5);
	s.rotate(90);
	l[0].rotate(90);
	Box e;
	s.envelope(e);
	cerr << bg::wkt(e) << endl;
	l[0].envelope(e);
	cerr << bg::wkt(e) << endl;
	*/
	vector<string> toPack;
	Layout l1(Parser::Parse("quadtreeTest.svg", toPack), 5);
	Layout l2(Parser::Parse("quadtreeTest.svg", toPack));
	Box e;
	l1[3].envelope(e);
	cerr << bg::wkt(e) << endl;
	l1[3].rotate(90);
	l1[3].translate(50, 100);
	l1[3].envelope(e);
	cerr << bg::wkt(e) << endl;
	cerr << Matrix(l1[3].getTransMatrix()) << endl;
	cerr << "L2" << endl;

	l2[3].envelope(e);
	cerr << bg::wkt(e) << endl;
	l2[3].rotate(90);
	l2[3].translate(50, 100);
	l2[3].envelope(e);
	cerr << bg::wkt(e) << endl;
	cerr << Matrix(l2[3].getTransMatrix()) << endl;

	Matrix m(0, 1, -1, 0, 50, 100);
	l1[3].applyMatrix(m);
	l2[3].applyMatrix(m);
	cerr << Matrix(l1[3].getTransMatrix()) << endl;
	cerr << Matrix(l2[3].getTransMatrix()) << endl;

	l1[3].restore();
	l2[3].restore();
	cerr << Matrix(l1[3].getTransMatrix()) << endl;
	cerr << Matrix(l2[3].getTransMatrix()) << endl;
    return EXIT_SUCCESS;
    }
