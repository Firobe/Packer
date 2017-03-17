#ifndef INNER_QUADTREE_HPP
#define INNER_QUADTREE_HPP

#include <iostream>

#include "common.hpp"
#include "Shape.hpp"
#include "bitmap.hpp"

class QuadTree;

enum color_enum {
    white,
    grey,
    black
};

/**
 * This class allow the creation of a quadTree based on a Shape or MultiPolygon
 */
class InnerQuadTree {
private:
    color_enum color;
    int depth;
    int size;    //stored quadTrees
    InnerQuadTree* q1, *q2, *q3, *q4;
    float x1, y1, x2, y2; //Tree bounding box

private:
    void copy(const InnerQuadTree&);
    InnerQuadTree(const InnerQuadTree&); //copy operator
    InnerQuadTree& operator=(const InnerQuadTree&); //assignment operator
    ~InnerQuadTree();

private:
    void construct(float x1, float y1, float x2, float y2, bitmap& bmap, int offsetX,
                   int offsetY, int length);
    InnerQuadTree(float x1, float y1, float x2, float y2, bitmap& bmap, int offsetX,
                  int offsetY, int size, int depth);

    friend class QuadTree;

    bool intersectsRec(const InnerQuadTree&, float offsetX, float offsetY, float offsetX2,
                       float offsetY2) const;
    void translater(float x, float y);

    std::vector<color_enum> getLine(int i, int depth);

public:
    friend std::ostream& operator<<(std::ostream&, const InnerQuadTree&);
};


#endif //INNER_QUADTREE_HPP
