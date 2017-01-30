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
#include <boost/geometry/algorithms/transform.hpp>
#include <boost/geometry/strategies/transform/matrix_transformers.hpp>

#include "InnerQuadTree.hpp"
#include "common.hpp"
#include "Shape.hpp"
#include "bitmap.hpp"

using namespace std;

void InnerQuadTree::copy(const InnerQuadTree &q) {
	color = q.color;
	depth = q.depth;
	size = q.size;
	q1 = q2 = q3 = q4 = nullptr;
	if (q.q1 != nullptr) q1 = new InnerQuadTree(*q.q1);
	if (q.q2 != nullptr) q2 = new InnerQuadTree(*q.q2);
	if (q.q3 != nullptr) q3 = new InnerQuadTree(*q.q3);
	if (q.q4 != nullptr) q4 = new InnerQuadTree(*q.q4);
	x1 = q.x1;
	y1 = q.y1;
	x2 = q.x2;
	y2 = q.y2;
}

InnerQuadTree::InnerQuadTree(const InnerQuadTree &q) {
	copy(q);
}

InnerQuadTree &InnerQuadTree::operator=(const InnerQuadTree &q) {
	if (this != &q) {
		this->~InnerQuadTree();
		copy(q);
	}
	return *this;
}

InnerQuadTree::~InnerQuadTree() {
	if (q1 != nullptr) delete q1;
	q1 = nullptr;
	if (q2 != nullptr) delete q2;
	q2 = nullptr;
	if (q3 != nullptr) delete q3;
	q3 = nullptr;
	if (q4 != nullptr) delete q4;
	q4 = nullptr;
}

void InnerQuadTree::construct(float x1, float y1, float x2, float y2, bitmap& bmap, int offsetX, int offsetY, int length) {
	// length, offsetX, offsetY are used in order to determine bitmap position corresponding to the tree
	// maxDepth is reach when length = 1

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
			double xm = (double) (x1 + x2)/2.0;
			double ym = (double) (y1 + y2)/2.0;

			// For each quarter of the defined area, compute the quadTree
			int half = length/2;
			q1 = new InnerQuadTree(x1, y1, xm, ym, bmap, offsetX, offsetY, half, depth+1); //bottom left
			q2 = new InnerQuadTree(x1, ym, xm, y2, bmap, offsetX, offsetY+half, half, depth+1); //bottom right
			q3 = new InnerQuadTree(xm, ym, x2, y2, bmap, offsetX+half, offsetY, half, depth+1); //top    right
			q4 = new InnerQuadTree(xm, y1, x2, ym, bmap, offsetX+half, offsetY+half, half, depth+1); //top    left
			size = q1->size + q2->size + q3->size + q4->size + 1;
		}
	}

	// If we are here it means, that the area is empty
}

InnerQuadTree::InnerQuadTree(float x1, float y1, float x2, float y2, bitmap& bmap, int offsetX, int offsetY, int length, int depth)
		: color(white), depth(depth), size(1),
		  q1(nullptr), q2(nullptr), q3(nullptr), q4(nullptr),
		  x1(x1), y1(y1), x2(x2), y2(y2) {
	construct(x1, y1, x2, y2, bmap, offsetX, offsetY, length);
}

bool InnerQuadTree::intersectsRec(const InnerQuadTree& q, float offsetX1, float offsetY1, float offsetX2, float offsetY2) const {
	if (color == white || q.color == white)
		return false;

	//float x1 = this->x1+0.2, x2 = this->x2+0.2, y1 = this->y1+0.2, y2 = this->y2+0.2;
	//float qx1 = q.x1+0.2, qx2 = q.x2+0.2, qy1 = q.y1+0.2, qy2 = q.y2+0.2;
	//bool boxIntersects = ((qx1 <= x1 && x1 <= qx2) || (x1 <= qx1 && qx1 <= x2)) && ((qy1 <= y1 && y1 <= qy2) || (y1 <= qy1 && qy1 <= y2));
	/*if (x2+offsetX1 < q.x1+offsetX2)
		return false;
	if (q.x2+offsetX2 < x1+offsetX1)
		return false;
	if (y2+offsetY1 < q.y1+offsetY2)
		return false;
	if (q.y2+offsetY2 < y1+offsetY1)
		return false;
*/

	Box b1{{x1+offsetX1,y1+offsetY1},{x2+offsetX1,y2+offsetY1}};
	Box b2{{q.x1+offsetX2,q.y1+offsetY2},{q.x2+offsetX2,q.y2+offsetY2}};
	bool boxIntersects = bg::intersects(b1, b2);
	//cout << boxIntersects << test << endl;

	if (!boxIntersects)
		return false;

	if (color == black && q.color == black) {
		cout << bg::wkt(b1) << " - " << bg::wkt(b2) << endl;
		return boxIntersects;
	}

	if (color == black && q.color == grey) {
		if (this->intersectsRec(*q.q1, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->intersectsRec(*q.q2, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->intersectsRec(*q.q3, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->intersectsRec(*q.q4, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		return false;
	}

	if (color == grey && q.color == black) {
		if (q.intersectsRec(*q1, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (q.intersectsRec(*q2, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (q.intersectsRec(*q3, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (q.intersectsRec(*q4, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		return false;
	}

	else {
		// In this case both nodes are grey
		if (this->q1->intersectsRec(*q.q1, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->q1->intersectsRec(*q.q2, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->q1->intersectsRec(*q.q3, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->q1->intersectsRec(*q.q4, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->q2->intersectsRec(*q.q1, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->q2->intersectsRec(*q.q2, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->q2->intersectsRec(*q.q3, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->q2->intersectsRec(*q.q4, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->q3->intersectsRec(*q.q1, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->q3->intersectsRec(*q.q2, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->q3->intersectsRec(*q.q3, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->q3->intersectsRec(*q.q4, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->q4->intersectsRec(*q.q1, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->q4->intersectsRec(*q.q2, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->q4->intersectsRec(*q.q3, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
		if (this->q4->intersectsRec(*q.q4, offsetX1, offsetY1, offsetX2, offsetY2))
			return true;
	}

	return false;
}

void InnerQuadTree::deepTranslater(float x, float y) {
	x1+=x;
	x2+=x;
	y1+=y;
	y2+=y;

	if (q1 != nullptr) {
		q1->deepTranslater(x, y);
		q2->deepTranslater(x, y);
		q3->deepTranslater(x, y);
		q4->deepTranslater(x, y);
	}
}


std::ostream& operator<<(std::ostream& s, const InnerQuadTree& q) {
	s << "Boundind box : (" << q.x1 << "," << q.y1 << ") (" << q.x2 << "," << q.y2 << ")" << endl;
	s << "Size : " << q.size << endl;
	return s;
}
