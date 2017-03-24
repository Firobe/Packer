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
    std::vector<InnerQuadTree*> trees;
    std::vector<Point> treesOffset;
    unsigned currentTree = 0;

    float _totalX, _totalY;
    int _maxDepth;
    float precision;


private:
    void copy(const QuadTree&);
public:
    QuadTree(const QuadTree&); //copy operator
    QuadTree& operator=(const QuadTree&); //assignment operator
    ~QuadTree();

public:
    QuadTree(Shape&, float precision);
    QuadTree(MultiPolygon& mult, float precision, unsigned id);

public:
    void translater(float x, float y);
    void rotater(float angle);

public:
    void rotate(double degrees) override;
    void translate(double Tx, double Ty) override;
    bool intersectsWith(const QuadTree&) const;

private:
    void saveTree(std::string filename, int depth);
public:
    void saveTree(std::string filename);

public:
    friend std::ostream& operator<<(std::ostream& s, const QuadTree&);
};

#endif // QUADTREE_HPP
