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
    QuadTree& operator=(const QuadTree&); //assignment operator
    QuadTree& operator=(QuadTree&& q);
    ~QuadTree();

public:
	QuadTree(Shape&, float precision);
	QuadTree(MultiPolygon& mult, float precision, unsigned id);

public:
	// Overrided algorithms
    void rotate(double degrees) override;
    void translate(double Tx, double Ty) override;
	bool intersectsWith(const Shape&) const override;
	void envelope(Box&) const override;
	int area() const override;

private:
    void saveTree(std::string filename, int depth);
public:
    void saveTree(std::string filename);

public:
    friend std::ostream& operator<<(std::ostream& s, const QuadTree&);
};

#endif // QUADTREE_HPP
