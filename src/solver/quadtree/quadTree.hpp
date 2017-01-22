#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include "common.hpp"
#include "Shape.hpp"
#include "bitmap.hpp"


/**
 * This class allow the creation of a quadTree base on a Shape
 */
class quadTree {
private:
    bool black;
    int depth; //We will be able to translate a quadTree only if depth == 0
    int size;    //store quadTree size (number of quadtrees)
    Point min_corner, max_corner;
    quadTree *q1, *q2, *q3, *q4;

    void copy(const quadTree&);

	void construct(double x1, double y1, double x2, double y2, bitmap& bmap, int offsetX, int offsetY, int length);
	quadTree(double x1, double y1, double x2, double y2, bitmap& bmap, int offsetX, int offsetY, int size, int length);

public:
    quadTree(Shape &shape, float precision);
    quadTree(const quadTree&); //copy operator
    quadTree& operator=(const quadTree&); //assignment operator
    ~quadTree();

};


#endif //QUADTREE_HPP
