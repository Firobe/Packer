#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include "InnerQuadTree.hpp"
#include "common.hpp"
#include "Shape.hpp"

class QuadTree
{
private:
	InnerQuadTree *tree;
	float _offsetX, _offsetY;
	float _angle;


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
};

#endif // QUADTREE_HPP
