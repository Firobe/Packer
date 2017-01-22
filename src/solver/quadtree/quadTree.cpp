#include <iostream>

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

using namespace std;

void quadTree::construct(double x1, double y1, double x2, double y2, Shape &shape, int maxDepth) {
	min_corner = Point(x1, y1);
	max_corner = Point(x2, y2);


	// Bounding box of this tree
	Box box(min_corner, max_corner);

	// if true, tree is black and if precision is not enought we can add new quadTrees
	if(bg::intersects(box, shape.getMultiP())) {
		black = true;

		// Compute intersection area in order to know if we need to stop tree construction or not
		MultiPolygon intersection;
	/*	bg::intersection(box, shape.getMultiP(), intersection);
		double area = bg::area(intersection);
*/
		// Create other quad tree if precision is not enought
		if (depth < maxDepth) {
			// Compute center points for defining each area
			double xm = (double) (min_corner.x() + max_corner.x())/2.0;
			double ym = (double) (min_corner.y() + max_corner.y())/2.0;

			// For each quarter of the defined area, compute the quadTree
			q1 = new quadTree(min_corner.x(), min_corner.y(), xm, ym, shape, precision, depth+1, maxDepth); //bottom left
			q2 = new quadTree(min_corner.x(), ym, xm, max_corner.y(), shape, precision, depth+1, maxDepth); //bottom right
			q3 = new quadTree(xm, ym, max_corner.x(), max_corner.y(), shape, precision, depth+1, maxDepth); //top    right
			q4 = new quadTree(xm, min_corner.y(), max_corner.x(), ym, shape, precision, depth+1, maxDepth); //top    left
			size = q1->size + q2->size + q3->size + q4->size + 1;
		}
	}

}

quadTree::quadTree(double x1, double y1, double x2, double y2, Shape &shape, float precision, int depth, int maxDepth)
		: black(false), depth(depth), precision(precision), size(1),
		  q1(nullptr), q2(nullptr), q3(nullptr), q4(nullptr) {
	construct(x1, y1, x2, y2, shape, maxDepth);
}

quadTree::quadTree(Shape &shape, float precision)
	: black(false), depth(0), precision(precision), size(1),
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
	// If the deaper quadTree represents a smaller area than precision, it's ok
	int maxDepth = 0;
	double area = (envelop.max_corner().x()-envelop.min_corner().x())*
			(envelop.max_corner().y()-envelop.min_corner().y());
	cout << "quadtree area : " << area << endl;
	while (area > precision) {
		area/=4.0;
		maxDepth++;
	}

	cout << "max depth : " << maxDepth << endl;

    // QuadTree size is shape envelop size
	construct(envelop.min_corner().y(), envelop.min_corner().y(),
			 envelop.max_corner().x(), envelop.max_corner().y(), shape, maxDepth);

	cout << "size : " << size << endl;
    // Restore shape position
    translate<Shape>(shape, reference.x(), reference.y());

}


void quadTree::copy(const quadTree &q) {
    min_corner = q.min_corner;
    max_corner = q.max_corner;
    black = q.black;
    precision = q.precision;
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
