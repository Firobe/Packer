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

void quadTree::construct(double x1, double y1, double x2, double y2, bitmap& bmap, int offsetX, int offsetY, int length) {
	// length, offsetX, offsetY are used in order to determine bitmap position corresponding to the tree
	// maxDepth is reach when size = 1
	// x1, x2, y1, y2 will be used for testing quadtrees intersection
	min_corner = Point(x1, y1);
	max_corner = Point(x2, y2);

	if (length == 1) {
		// Stoping case
		black = bmap.get(offsetX, offsetY);
		return;
	}

	if (bmap.hasBlack(offsetX, offsetY, length)) {
		// Int this case, case is black and we may create new quadtrees only if current area is not full white
		black = true;

		if (bmap.hasWhite(offsetX, offsetY, length)) {
			// If bmap is not fully black, we need to create more quadtrees for more precision

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
		: black(false), depth(depth), size(1),
		  q1(nullptr), q2(nullptr), q3(nullptr), q4(nullptr) {
	construct(x1, y1, x2, y2, bmap, offsetX, offsetY, length);
}

quadTree::quadTree(Shape &shape, float precision)
	// presision will be the width or height minimal that the quadtree should detect
	: black(false), depth(0), size(1),
	  q1(nullptr), q2(nullptr), q3(nullptr), q4(nullptr) {

    // Compute the shape Box envelop
    Box envelop;
    bg::envelope(shape.getMultiP(), envelop);
    bg::correct(envelop);
    Point reference = envelop.min_corner();

    // Place the shape into the (0,0) point in order to create the quadTree
    translate<Shape>(shape, -reference.x(), -reference.y());
    translate<Box>(envelop, -reference.x(), -reference.y());

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
    translate<Shape>(shape, reference.x(), reference.y());

}


void quadTree::copy(const quadTree &q) {
    min_corner = q.min_corner;
    max_corner = q.max_corner;
	black = q.black;
    depth = q.depth;
    q1 = q2 = q3 = q4 = nullptr;
    if (q.q1 != nullptr) q1 = new quadTree(*q.q1);
    if (q.q2 != nullptr) q2 = new quadTree(*q.q2);
    if (q.q3 != nullptr) q3 = new quadTree(*q.q3);
    if (q.q4 != nullptr) q4 = new quadTree(*q.q4);
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
