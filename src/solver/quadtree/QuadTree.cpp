#include <cmath>
#include <iostream>
#include <fstream>

#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/intersects.hpp>

#include "bitmap.hpp"
#include "QuadTree.hpp"
#include "InnerQuadTree.hpp"
#include "common.hpp"
#include "Shape.hpp"

using namespace std;

/**
 * @brief QuadTree::copy copy the content of q into this
 * @param q
 */
void QuadTree::copy(const QuadTree &q) {
	cout << "quadtree copied" << endl;
	tree = new InnerQuadTree(*q.tree);
	bmap = new bitmap(*q.bmap);
	_offsetX=q._offsetX;
	_offsetY=q._offsetY;
	_angle=q._angle;
	_maxDepth=q._maxDepth;
}

/**
 * @brief copy constructor
 * @param q
 */
QuadTree::QuadTree(const QuadTree &q) {
	copy(q);
}

/**
 * @brief assignement operator
 * @param q
 * @return the quadtree copied
 */
QuadTree &QuadTree::operator=(const QuadTree &q) {
	if (this != &q) {
		this->~QuadTree();
		copy(q);
	}
	return *this;
}

/**
 * @brief destructor
 */
QuadTree::~QuadTree() {
	if (tree != nullptr) delete tree;
	tree = nullptr;
	if (bmap != nullptr) delete bmap;
	bmap = nullptr;
}

/**
 * @brief Constructor for Shapes, it just call the MultiPolygon constructor with the Shape's MultiPolygon
 * @param s shape that will be used for the construction
 * @param precision is the precision of the quadtree constructed, it mean that any shape form larger
   than precision can be detected by the quadtree
 * @param offsetX is optional and allow to translate the initial X position of the QuadTree
 * @param offsetY is optional and allow to translate the initial Y position of the QuadTree
 * @param angle is optional and allow to rotate the initial angle of the QuadTree
 */
QuadTree::QuadTree(Shape &s, float precision, float offsetX, float offsetY, float angle) :
	QuadTree(s.getMultiP(), precision, offsetX, offsetY, angle) {
	bmap->saveMap(s.getID());
}


/**
 * @brief QuadTree::QuadTree is the main constructor of the QuadTree, it will create a bitmap according to the
   desired precision and will use it in order to generate a region QuadTree that represent the given MultiPolygon
 * @param mult is the MultiPolygon that will be transformed into a QuadTree
 * @param precision is the precision of the quadtree constructed, it mean that any shape form larger
   than precision can be detected by the quadtree
 * @param offsetX is optional and allow to translate the initial X position of the QuadTree
 * @param offsetY is optional and allow to translate the initial Y position of the QuadTree
 * @param angle is optional and allow to rotate the initial angle of the QuadTree
 */
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
	_maxDepth = maxDepth;
	int size = pow(2, maxDepth);
	bmap = new bitmap(mult,size,size);

	// QuadTree size is shape envelop size
	tree = new InnerQuadTree(envelop.min_corner().x(), envelop.min_corner().y(),
			 envelop.max_corner().x(), envelop.max_corner().y(),
			 *bmap, 0, 0, size, 0);

	// Restore shape position
	translate<MultiPolygon>(mult, reference.x(), reference.y());

}

/**
 * @brief QuadTree::intersects detect if two QuadTree intersects each other
 * @param q
 * @return
 */
bool QuadTree::intersects(const QuadTree&q) const {
	return tree->intersectsRec(*q.tree, _offsetX, _offsetY, q._offsetX, q._offsetY);
}

/**
 * @brief QuadTree::translater translates the QuadTree position
 * @param x
 * @param y
 */
void QuadTree::translater(float x, float y) {
	if (!rotated) {
		// in this case, movement is easy to do
		_offsetX+=x;
		_offsetY+=y;
		moved = true;
	} else {
		// in this case we need to transform the old_tr*old_rot*new_tra
		// into a translation*rotation movement
		float newx, newy;
		newx = _offsetX + x*cos(_angle) + y*sin(_angle);
		newy = _offsetY - x*sin(_angle) + y*cos(_angle);
		_offsetX = newx;
		_offsetY = newy;
		moved = true;
	}
}

/**
 * @brief QuadTree::applyTranslation apply the stored translation
 * @return true if something was done, false if nothing had to be done or in case of error
 */
bool QuadTree::applyTranslation() {
	if (!moved)
		return false;
	tree->translater(_offsetX, _offsetY);
	_offsetX = 0;
	_offsetY = 0;
	moved = false;
}

/**
 * @brief QuadTree::rotate rotate a QuadTree
 * The rotation is around the (0,0) point by default
 * Better rotation is possible with the possibility to choose the rotation point
 * @param angle rotation angle in degree, stored in radians
 */
void QuadTree::rotate(float angle) {
	_angle += M_PI * angle / 180.0;
	rotated = true;
}

/**
 * @brief QuadTree::applyTranslation apply the stored rotation
 * If translation was not before, apply it first
 * @return true if something was done, false if nothing had to be done or in case of error
 */
bool QuadTree::applyRotation() {
	// Tree position and size
	float posx = _offsetX + tree->x1;
	float posy = _offsetY + tree->y1;
	float width = tree->x2 - tree->x1;
	float height = tree->y2 - tree->y1;

	// First we apply a static rotation to the bitmap as if the bitmap is at the (0,0) coordinate
	bitmap* bmap2 = bitmap::rotate(bmap, _angle);
	delete bmap;
	bmap = bmap2;

	// TODO : trimm with a offset calculation, so we can't trimm the bitmap now

	// Determine tree max depth according to the bitmap size
	int size = pow(2, max(ceil(log2(bmap2->getHeight())), ceil(log2(bmap2->getWidth()))));

	// Compute good translation to apply to the new QuadTree creation
	float tx = posx*cos(_angle) - posy*sin(_angle);
	float ty = posx*sin(_angle) + posy*cos(_angle);

	delete tree;
	tree = new InnerQuadTree(tx, ty, tx+width, ty+height,*bmap, 0, 0, size, 0);

	// Fresh tree, positions are reseted
	rotated = false;
	moved = false;
	_offsetX = 0.0;
	_offsetY = 0.0;
	_angle = 0.0;

	return true;
}

/**
 * @brief operator << get informations about the QuadTree for debugging purpose
 * @param s
 * @param q
 */
std::ostream& operator <<(std::ostream& s, const QuadTree& q) {
	s << "Position : (" << q._offsetX << "," << q._offsetY << ")" << std::endl;
	s << "Angle : " << q._angle << std::endl;
	s << " - Tree : " << std::endl << *q.tree;
	s << " - bitmap : " << std::endl << *q.bmap;
	return s;
}

/**
 * @brief QuadTree::saveTree save the depth QuadTree in the PGM format
 * @param filename base name of the file, the full name will be : filename.pbm
 * @param depth
 */
void QuadTree::saveTree(std::string filename, int depth) {
	int size = pow(2, depth);

	ofstream file;
	file.open(filename + ".pgm");
	file << "P2" << endl;
	file << size << " " << size << endl;
	file << "2" << endl;

	for (int i=0; i<size; i++) {
		std::vector<color_enum> vec = tree->getLine(i, depth);
		//cout << vec.size() << " - " << size << endl;
		for (color_enum color : vec) {
			switch (color) {
			case white:
				file << "2 ";
				break;
			case grey:
				file << "1 ";
				break;
			default:
				file << "0 ";
				break;
			}
		}
		file << endl;
	}
	file.close();
}

/**
 * @brief QuadTree::saveTree save the any QuadTree's depth in the PGM format
 * @param filename base name of the file, the full name will be : filename-depth.pbm
 */
void QuadTree::saveTree(std::string filename) {
	cout << _maxDepth << endl;
	for (int i=0; i<=_maxDepth; i++){
		saveTree(filename + "-" + std::to_string(i), i);
	}
}
