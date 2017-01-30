#include <cmath>

#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/intersects.hpp>

#include "bitmap.hpp"
#include "QuadTree.hpp"
#include "InnerQuadTree.hpp"
#include "common.hpp"
#include "Shape.hpp"


void QuadTree::copy(const QuadTree &q) {
	tree = new InnerQuadTree(*q.tree);
	bmap = new bitmap(*q.bmap);
	_offsetX=q._offsetX;
	_offsetY=q._offsetY;
	_angle=q._angle;
}

QuadTree::QuadTree(const QuadTree &q) {
	copy(q);
}

QuadTree &QuadTree::operator=(const QuadTree &q) {
	if (this != &q) {
		this->~QuadTree();
		copy(q);
	}
	return *this;
}

QuadTree::~QuadTree() {
	if (tree != nullptr) delete tree;
	tree = nullptr;
	if (bmap != nullptr) delete bmap;
	bmap = nullptr;
}

QuadTree::QuadTree(Shape &s, float precision, float offsetX, float offsetY, float angle) :
	QuadTree(s.getMultiP(), precision, offsetX, offsetY, angle) {}

QuadTree::QuadTree(MultiPolygon &mult, float precision, float offsetX, float offsetY, float angle) :
	tree(nullptr), _offsetX(offsetX), _offsetY(offsetY), _angle(angle){

	// Compute the shape Box envelop
	Box envelop;
	bg::envelope(mult, envelop);
	bg::correct(envelop);
	Point reference = envelop.min_corner();
	_offsetX+=reference.x();
	_offsetY+=reference.y();

	// Place the shape into the (0,0) point in order to create the quadTree
	translate<MultiPolygon>(mult, -reference.x(), -reference.y());
	translate<Box>(envelop, -reference.x(), -reference.y());

	// We determine maxDepth thanks to the precision
	// If the deaper quadTree width and height need to be smaller than precision
	int maxDepth = 0;
	float width = envelop.max_corner().x();
	float height = envelop.max_corner().y();
	while (width > precision || height > precision) {
		width/=2;
		height/=2;
		maxDepth++;
	}

	// Create the bitmap that will be used in order to create the quadTree
	int size = pow(2, maxDepth);
	bmap = new bitmap(mult,size,size);
	// bmap.saveMap(shape.getID());

	// QuadTree size is shape envelop size
	tree = new InnerQuadTree(envelop.min_corner().y(), envelop.min_corner().y(),
			 envelop.max_corner().x(), envelop.max_corner().y(),
			 *bmap, 0, 0, size, 0);

	// Restore shape position
	translate<MultiPolygon>(mult, reference.x(), reference.y());

	/*
	std::cout << offsetX << ":" << offsetY << std::endl;
	cout << "max depth : " << maxDepth << endl;
	cout << "Shape ID :" << shape.getID() << endl;
	cout << "shape area : " << bg::area(shape.getMultiP()) << endl;
	cout << "size : " << size << endl;
	cout << "quadtrees : " << this->size << endl << endl;	*/
}

bool QuadTree::intersects(const QuadTree&q) const {
	return tree->intersectsRec(*q.tree, _offsetX, _offsetY, q._offsetX, q._offsetY);
}

void QuadTree::translater(float x, float y) {
	_offsetX+=x;
	_offsetY+=y;
}


std::ostream& operator <<(std::ostream& s, const QuadTree& q) {
	s << "Position : (" << q._offsetX << "," << q._offsetY << ")" << std::endl;
	s << "Angle : " << q._angle << std::endl;
	s << " - Tree : " << std::endl << *q.tree;
	s << " - bitmap : " << std::endl << *q.bmap;
	return s;
}
