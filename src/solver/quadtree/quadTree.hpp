#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include "common.hpp"
#include "Shape.hpp"
#include "bitmap.hpp"

enum color_enum {
	white,
	grey,
	black
};


/**
 * This class allow the creation of a quadTree base on a Shape
 */
class quadTree {
private:
	color_enum color;
    int depth; //We will be able to translate a quadTree only if depth == 0
    int size;    //store quadTree size (number of quadtrees)
    Point min_corner, max_corner;
    quadTree *q1, *q2, *q3, *q4;
	double _offsetX, _offsetY;

private:
    void copy(const quadTree&);
public:
	quadTree(const quadTree&); //copy operator
	quadTree& operator=(const quadTree&); //assignment operator
	~quadTree();

private:
	void construct(double x1, double y1, double x2, double y2, bitmap& bmap, int offsetX, int offsetY, int length);
	quadTree(double x1, double y1, double x2, double y2, bitmap& bmap, int offsetX, int offsetY, int size, int length);
public:
    quadTree(Shape &shape, float precision);

	bool layerIntersects(const quadTree&) const;
	bool intersectsRec(const quadTree&, double offsetX, double offsetY, double offsetX2, double offsetY2) const;
	bool intersects(const quadTree&) const;

	void translater(double x, double y);

};


#endif //QUADTREE_HPP
