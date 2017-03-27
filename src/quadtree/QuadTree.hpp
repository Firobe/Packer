#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <string>
#include <iostream>
#include <vector>

#include "InnerQuadTree.hpp"
#include "common.hpp"
#include "Shape.hpp"

class bitmap;

class QuadTree : public Shape {
private:
    static constexpr unsigned quadsNumber = 12; //30 degree angle
    InnerQuadTree** trees;
    std::vector<Point> treesOffset;
	std::vector<Point> _centroid;
    unsigned currentTree = 0;

    float _totalX, _totalY;
    int _maxDepth;
	float precision;
	int _area;

private:
    void copy(const QuadTree&);
    void destroy();
public:
	QuadTree(const QuadTree&); //copy operator
	QuadTree& operator=(const Shape&) override; //assignment operator
    QuadTree& operator=(QuadTree&& q);
    ~QuadTree();

public:
	QuadTree(Shape&, float precision);
	QuadTree(MultiPolygon& mult, float precision, unsigned id);

public:
	// Overrided algorithms
	/*void mergeWith(const Shape&) {
		std::cerr << "mergeWith Not implemented" << std::endl;
	}*/
	void rotate(double degrees) override;
	void translate(double Tx, double Ty) override;
	void envelope(Box&) const override;
	Point centroid() const override;
	int area() const override;
	bool intersectsWith(const Shape&) const override;
	bool intersectsWith(const Ring&) const {
		std::cerr << "intersectsWith Not implemented" << std::endl;
		return false;
	}
	void convexHull(Polygon&) const {
		std::cerr << "convexHull Not implemented" << std::endl;
	}
	void bufferize(double) {
		std::cerr << "bufferize Not implemented" << std::endl;
		return;
	}
	/*std::array<double, 6> getTransMatrix() const {
		std::cerr << "getTransMatrix Not implemented" << std::endl;
		std::array<double, 6> result;
		return result;
	}*/

private:
    void saveTree(std::string filename, int depth);
public:
    void saveTree(std::string filename);

public:
    friend std::ostream& operator<<(std::ostream& s, const QuadTree&);
};

#endif // QUADTREE_HPP
