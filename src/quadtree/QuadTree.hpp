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

	float _oldX, _oldY;

	float _backupX, _backupY;
	unsigned _backupPos;

private:
    void copy(const QuadTree&);
    void destroy();
public:
    QuadTree() {
        trees = nullptr;
    };
    QuadTree(const QuadTree&); //copy operator
    QuadTree& operator=(const Shape&) override; //assignment operator
    QuadTree& operator=(Shape&& q) override;
    ~QuadTree();

public:
    QuadTree(Shape&, float precision);
    QuadTree(MultiPolygon& mult, float precision, unsigned id);

public:
    // Overriden algorithms
	void savePos(); // TODO : implement it
	void restorePos(); // TODO : implement it
	void mergeWith(const Shape&) override;
    void rotate(double degrees) override;
    void translate(double Tx, double Ty) override;
    void envelope(Box&) const override;
    Point centroid() const override;
    int area() const override;
    bool intersectsWith(const Shape&) const override;
	bool intersectsWith(const Ring&) const override;
	void convexHull(Polygon&) const override;
	void bufferize(double) override;
	std::array<double, 6> getTransMatrix() const override;

private:
    void saveTree(std::string filename, int depth);
	void construct(float precision, MultiPolygon mult);
	void construct(float precision, Shape& s);

public:
    void saveTree(std::string filename);

public:
    friend std::ostream& operator<<(std::ostream& s, const QuadTree&);
};

#endif // QUADTREE_HPP
