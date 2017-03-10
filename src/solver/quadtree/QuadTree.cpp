#include <cmath>
#include <iostream>
#include <fstream>


#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/intersects.hpp>
#include <boost/math/constants/constants.hpp>

#include "bitmap.hpp"
#include "QuadTree.hpp"
#include "InnerQuadTree.hpp"
#include "common.hpp"
#include "Shape.hpp"

using namespace std;

static float pi = boost::math::constants::pi<float>();

/**
 * @brief QuadTree::copy copy the content of q into this
 * @param q
 */
void QuadTree::copy(const QuadTree &q) {
	cout << "quadtree copied" << endl;
	tree = new InnerQuadTree(*q.tree);
	bmap = new bitmap(*q.bmap);
	_currentX=q._currentX;
	_currentY=q._currentY;
	_currentAngle=q._currentAngle;
	_totalX = q._totalX;
	_totalY = q._totalY;
	_totalAngle = q._totalAngle;
	_maxDepth=q._maxDepth;
	multiP = q.multiP;
	rotated = q.rotated;
	moved = q.moved;
	precision = q.precision;
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
	tree(nullptr), multiP(mult), precision(precision) {

	// Compute the shape Box envelop
	Box envelop;
	bg::envelope(multiP, envelop);
	bg::correct(envelop);
	Point reference = envelop.min_corner();
	_totalX = reference.x() + offsetX;
	_totalY = reference.y() + offsetY;
	_totalAngle = angle;
	_currentX = 0.0;
	_currentY = 0.0;
	_currentAngle = 0.0; //TODO : apply rotation to the MultiP
	moved = rotated = false;

	// Place the shape into the (0,0) point in order to create the quadTree
	translate<MultiPolygon>(multiP, -reference.x(), -reference.y());
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
	tree = new InnerQuadTree(envelop.min_corner().x()+_totalX, envelop.min_corner().y()+_totalY,
			 envelop.max_corner().x()+_totalX, envelop.max_corner().y()+_totalY,
			 *bmap, 0, 0, size, 0);

}

/**
 * @brief QuadTree::intersects detect if two QuadTree intersects each other
 * @param q
 * @return
 */
bool QuadTree::intersects(const QuadTree&q) const {
	return tree->intersectsRec(*q.tree, _currentX, _currentY, q._currentX, q._currentY);
}

/**
 * @brief QuadTree::translater translates the QuadTree position
 * @param x
 * @param y
 */
void QuadTree::translater(float x, float y) {
	// we need to transform the old_tr*old_rot*new_tra
	// into a translation*rotation movement
	float newx, newy;
	newx = _currentX + x*cos(_currentAngle) + y*sin(_currentAngle);
	newy = _currentY - x*sin(_currentAngle) + y*cos(_currentAngle);
	_currentX = newx;
	_currentY = newy;

	// Global translation computation
	newx = _totalX + x*cos(_totalAngle) + y*sin(_totalAngle);
	newy = _totalY - x*sin(_totalAngle) + y*cos(_totalAngle);
	_totalX = newx;
	_totalY = newy;

	moved = true;
}

/**
 * @brief QuadTree::applyTranslation apply the stored translation
 * @return true if something was done, false if nothing had to be done or in case of error
 */
bool QuadTree::applyTranslation() {
	if (!moved)
		return false;
	tree->translater(_currentX, _currentY);
	_currentX = 0;
	_currentY = 0;
	moved = false;
	return true;
}

/**
 * @brief QuadTree::rotate rotate a QuadTree
 * The rotation is around the (0,0) point by default
 * Better rotation is possible with the possibility to choose the rotation point
 * @param angle rotation angle in degree, stored in radians
 */
void QuadTree::rotater(float angle) {
	_currentAngle += pi * angle / 180.0;
	_totalAngle += pi * angle / 180.0;
	rotated = true;
}

/**
 * @brief QuadTree::applyTranslation apply the stored rotation
 * If translation was not before, apply it first
 * @return true if something was done, false if nothing had to be done or in case of error
 */
bool QuadTree::applyRotation() {

	// Compute the tree position
	float newX = cos(_totalAngle)*_totalX - sin(_totalAngle)*_totalY;
	float newY = sin(_totalAngle)*_totalX + cos(_totalAngle)*_totalY;

	// Rotate the MultiPolygon
	MultiPolygon newP;
	bg::strategy::transform::rotate_transformer<bg::radian, float, 2, 2> rotator(_totalAngle);
	bg::transform(multiP, newP, rotator);

	// Get it to the (0,0) point in order to create the QuadTree
	Box envelop;
	bg::envelope(newP, envelop);
	bg::correct(envelop);
	Point reference = envelop.min_corner();
	translate<MultiPolygon>(newP, -reference.x(), -reference.y());
	translate<Box>(envelop, -reference.x(), -reference.y());

	// Compute additional translation
	newX += reference.x();
	newY += reference.y();

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
	delete bmap;
	bmap = new bitmap(newP,size,size);

	// QuadTree size is shape envelop size
	tree = new InnerQuadTree(envelop.min_corner().x()+newX, envelop.min_corner().y()+newY,
			 envelop.max_corner().x()+newX, envelop.max_corner().y()+newY,
			 *bmap, 0, 0, size, 0);

	// Refresh tree, positions are reseted
	rotated = false;
	moved = false;
	_currentX = 0.0;
	_currentY = 0.0;
	_currentAngle = 0.0;

	return true;
}

/**
 * @brief operator << get informations about the QuadTree for debugging purpose
 * @param s
 * @param q
 */
std::ostream& operator <<(std::ostream& s, const QuadTree& q) {
	s << "Absolute Position : (" << q._totalX << "," << q._totalY << ") " << endl;//std::boolalpha << q.moved << std::endl;
	s << "Absolute angle : " << q._totalAngle << endl;
	s << "Relative position : (" << q._currentX << "," << q._currentY << ")" << endl;
	s << "Relative angle : " << q._currentAngle << endl;
	s << "Angle : " << q._currentAngle <<  " " << std::boolalpha << q.rotated << std::endl;
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
