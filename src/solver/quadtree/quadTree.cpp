#include <iostream>
#include <cmath>

#include <boost/geometry.hpp>
// Plante sans cette include pour une raison indeterminé

#include <boost/geometry/io/wkt/write.hpp>
#include <boost/geometry/io/wkt/read.hpp>
#include <boost/geometry/algorithms/envelope.hpp>
#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/algorithms/intersects.hpp>
#include <boost/geometry/algorithms/intersection.hpp>

#include "quadTree.hpp"
#include "common.hpp"
#include "Shape.hpp"
#include "bitmap.hpp"

using namespace std;

void quadTree::copy(const quadTree &q) {
	min_corner = q.min_corner;
	max_corner = q.max_corner;
	color = q.color;
	depth = q.depth;
	q1 = q2 = q3 = q4 = nullptr;
	if (q.q1 != nullptr) q1 = new quadTree(*q.q1);
	if (q.q2 != nullptr) q2 = new quadTree(*q.q2);
	if (q.q3 != nullptr) q3 = new quadTree(*q.q3);
	if (q.q4 != nullptr) q4 = new quadTree(*q.q4);
	_offsetX = q._offsetX;
	_offsetY = q._offsetY;
}

quadTree::quadTree(const quadTree &q) {
	copy(q);
}

quadTree &quadTree::operator=(const quadTree &q) {
	if (this != &q) {
		this->~quadTree();
		copy(q);
	}
	return *this;
}

quadTree::~quadTree() {
	if (q1 != nullptr) delete q1;
	q1 = nullptr;
	if (q2 != nullptr) delete q2;
	q2 = nullptr;
	if (q3 != nullptr) delete q3;
	q3 = nullptr;
	if (q4 != nullptr) delete q4;
	q4 = nullptr;
}

void quadTree::construct(double x1, double y1, double x2, double y2, bitmap& bmap, int offsetX, int offsetY, int length) {
	// length, offsetX, offsetY are used in order to determine bitmap position corresponding to the tree
	// maxDepth is reach when size = 1
	// x1, x2, y1, y2 will be used for testing quadtrees intersection
	min_corner = Point(x1, y1);
	max_corner = Point(x2, y2);

	if (length == 1) {
		// Stoping case, leaf canno't be grey
		if(bmap.get(offsetX, offsetY))
			color = black;
		else
			color = white;
		return;
	}

	if (bmap.hasBlack(offsetX, offsetY, length)) {
		// Int this case, case is black and we may create new quadtrees only if current area is not full white
		color = black;

		if (bmap.hasWhite(offsetX, offsetY, length)) {
			// If bmap is not fully black, we need to create more quadtrees for more precision
			color = grey;

			// Compute center points for defining each area
			// This information is not usefull for the quadTree creation, only for quadtrees intersections
			double xm = (double) (min_corner.x() + max_corner.x())/2.0;
			double ym = (double) (min_corner.y() + max_corner.y())/2.0;

			// For each quarter of the defined area, compute the quadTree
			int half = length/2;
			q1 = new quadTree(min_corner.x(), min_corner.y(), xm, ym, bmap, offsetX, offsetY, half, depth+1); //bottom left
			q2 = new quadTree(min_corner.x(), ym, xm, max_corner.y(), bmap, offsetX, offsetY+half, half, depth+1); //bottom right
			q3 = new quadTree(xm, ym, max_corner.x(), max_corner.y(), bmap, offsetX+half, offsetY, half, depth+1); //top    right
			q4 = new quadTree(xm, min_corner.y(), max_corner.x(), ym, bmap, offsetX+half, offsetY+half, half, depth+1); //top    left
			size = q1->size + q2->size + q3->size + q4->size + 1;
		}
	}

	// If we are here it means, that the area is empty
}

quadTree::quadTree(double x1, double y1, double x2, double y2, bitmap& bmap, int offsetX, int offsetY, int length, int depth)
		: color(white), depth(depth), size(1),
		  q1(nullptr), q2(nullptr), q3(nullptr), q4(nullptr) {
	construct(x1, y1, x2, y2, bmap, offsetX, offsetY, length);
}

quadTree::quadTree(Shape &shape, float precision)
	// presision will be the width or height minimal that the quadtree should detect
	: color(white), depth(0), size(1),
	  q1(nullptr), q2(nullptr), q3(nullptr), q4(nullptr),
	  _offsetX(0.0), _offsetY(0.0) {

    // Compute the shape Box envelop
    Box envelop;
    bg::envelope(shape.getMultiP(), envelop);
    bg::correct(envelop);
    Point reference = envelop.min_corner();

    // Place the shape into the (0,0) point in order to create the quadTree
    translate<Shape>(shape, -reference.x(), -reference.y());
    translate<Box>(envelop, -reference.x(), -reference.y());

	cout << "Shape ID :" << shape.getID() << endl;
	cout << "shape area : " << bg::area(shape.getMultiP()) << endl;

	// We determine maxDepth thanks to the precision
	// If the deaper quadTree width and height need to be smaller than precision
	int maxDepth = 0;

	double width = envelop.max_corner().x();
	double height = envelop.max_corner().y();
	while (width > precision || height > precision) {
		width/=2;
		height/=2;
		maxDepth++;
	}

	// Create the bitmap that will be used in order to create the quadTree
	int size = pow(2, maxDepth);
	bitmap bmap(shape,size,size);
	bmap.saveMap(shape.getID());

	cout << "max depth : " << maxDepth << endl;

	// QuadTree size is shape envelop size
	construct(envelop.min_corner().y(), envelop.min_corner().y(),
			 envelop.max_corner().x(), envelop.max_corner().y(),
			 bmap, 0, 0, size);

	cout << "size : " << size << endl;
	cout << "quadtrees : " << this->size << endl << endl;

	// Restore shape position
	//translate<Shape>(shape, reference.x(), reference.y());

}


bool quadTree::layerIntersects(const quadTree& q) const {
	bool boxIntersects = bg::intersects(Box(min_corner, max_corner), Box(q.min_corner, q.max_corner)); // faster
	//cout << boxIntersects << bg::intersects(Box(min_corner, max_corner), Box(q.min_corner, q.max_corner)) << endl;

	if (!boxIntersects)
		return false;

	if (color == black && q.color == black)
		return boxIntersects;

	return false;
}

bool quadTree::intersectsRec(const quadTree& q, double offsetX1, double offsetY1, double offsetX2, double offsetY2) const {

	if (color == white || q.color == white)
		return false;

	//double x1 = min_corner.x()+0.2, x2 = max_corner.x()+0.2, y1 = min_corner.y()+0.2, y2 = max_corner.y()+0.2;
	//double qx1 = q.min_corner.x()+0.2, qx2 = q.max_corner.x()+0.2, qy1 = q.min_corner.y()+0.2, qy2 = q.max_corner.y()+0.2;
	//bool boxIntersects = ((qx1 <= x1 && x1 <= qx2) || (x1 <= qx1 && qx1 <= x2)) && ((qy1 <= y1 && y1 <= qy2) || (y1 <= qy1 && qy1 <= y2));

	Box b1(Point(min_corner.x()+offsetX1, min_corner.y()+offsetY1), Point(max_corner.x()+offsetX1, max_corner.y()+offsetY1));
	Box b2(Point(q.min_corner.x()+offsetX2, q.min_corner.y()+offsetY2), Point(q.max_corner.x()+offsetX2, q.max_corner.y()+offsetY2));
	bool boxIntersects = bg::intersects(b1, b2); // faster
	//cout << boxIntersects << bg::intersects(Box(min_corner, max_corner), Box(q.min_corner, q.max_corner)) << endl;

	if (!boxIntersects)
		return false;

	if (color == black && q.color == black)
		return boxIntersects;

	if (color == black && q.color == grey) {
		if (this->intersectsRec(*q.q1, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->intersectsRec(*q.q2, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->intersectsRec(*q.q3, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->intersectsRec(*q.q4, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		return false;
	}

	if (color == grey && q.color == black) {
		if (q.intersectsRec(*q1, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (q.intersectsRec(*q2, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (q.intersectsRec(*q3, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (q.intersectsRec(*q4, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		return false;
	}

	else {
		// In this case both nodes are grey
		if (this->q1->intersectsRec(*q.q1, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->q1->intersectsRec(*q.q2, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->q1->intersectsRec(*q.q3, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->q1->intersectsRec(*q.q4, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->q2->intersectsRec(*q.q1, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->q2->intersectsRec(*q.q2, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->q2->intersectsRec(*q.q3, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->q2->intersectsRec(*q.q4, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->q3->intersectsRec(*q.q1, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->q3->intersectsRec(*q.q2, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->q3->intersectsRec(*q.q3, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->q3->intersectsRec(*q.q4, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->q4->intersectsRec(*q.q1, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->q4->intersectsRec(*q.q2, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->q4->intersectsRec(*q.q3, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
		if (this->q4->intersectsRec(*q.q4, offsetX1, offsetY1, offsetX2, offsetY2)) return true;
	}

	return false;
}

bool quadTree::intersects(const quadTree &q) const {
	return intersectsRec(q, _offsetX, _offsetY, q._offsetX, q._offsetY);
}

void quadTree::translater(double x, double y) {
	if (this->depth != 0) throw "Error";
	_offsetX+=x;
	_offsetY+=y;
}
