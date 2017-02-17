#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <string>
#include <iostream>

#include "InnerQuadTree.hpp"
#include "common.hpp"
#include "Shape.hpp"

class bitmap;

class QuadTree
{
private:
	InnerQuadTree *tree;
	float _currentX, _currentY, _currentAngle;
	float _totalX, _totalY, _totalAngle;
	bitmap *bmap;
	MultiPolygon multiP;
	int _maxDepth;
	bool rotated, moved;
	float precision;


private:
	void copy(const QuadTree&);
public:
	QuadTree(const QuadTree&); //copy operator
	QuadTree& operator=(const QuadTree&); //assignment operator
	~QuadTree();

public:
	QuadTree(Shape &, float precision, float offsetX=0.0, float offsetY=0.0, float angle=0.0);
	QuadTree(MultiPolygon &mult, float precision, float offsetX=0.0, float offsetY=0.0, float angle=0.0);

public:
	bool intersects(const QuadTree&) const;
	void translater(float x, float y);
	bool applyTranslation();
	void rotater(float angle);
	bool applyRotation();

private:
	void saveTree(std::string filename, int depth);
public:
	void saveTree(std::string filename);

public:
	friend std::ostream& operator<<(std::ostream& s, const QuadTree&);
};

#endif // QUADTREE_HPP
