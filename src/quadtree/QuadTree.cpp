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
void QuadTree::copy(const QuadTree& q) {
    cout << "quadtree copied" << endl;
    trees.reserve(quadsNumber);

    for (InnerQuadTree* quad : q.trees)
        trees.push_back(new InnerQuadTree(*quad));

    treesOffset = q.treesOffset;
    bmap = new bitmap(*q.bmap);
    _totalX = q._totalX;
    _totalY = q._totalY;
    _maxDepth = q._maxDepth;
    multiP = q.multiP;
    precision = q.precision;
}

/**
 * @brief copy constructor
 * @param q
 */
QuadTree::QuadTree(const QuadTree& q) {
    copy(q);
}

/**
 * @brief assignement operator
 * @param q
 * @return the quadtree copied
 */
QuadTree& QuadTree::operator=(const QuadTree& q) {
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
    for (InnerQuadTree* quad : trees) {
        if (quad != nullptr)
            delete quad;

        quad = nullptr;
    }

    if (bmap != nullptr) delete bmap;

    bmap = nullptr;
}

/**
 * @brief Constructor for Shapes, it just call the MultiPolygon constructor with the Shape's MultiPolygon
 * @param s shape that will be used for the construction
 * @param precision is the precision of the quadtree constructed, it mean that any shape form larger
   than precision can be detected by the quadtree
 */
QuadTree::QuadTree(Shape& s, float precision) :
    QuadTree(s._multiP, precision) {
    bmap->saveMap(s.getIdentifier());
}


/**
 * @brief QuadTree::QuadTree is the main constructor of the QuadTree, it will create a bitmap according to the
   desired precision and will use it in order to generate a region QuadTree that represent the given MultiPolygon
 * @param mult is the MultiPolygon that will be transformed into a QuadTree
 * @param precision is the precision of the quadtree constructed, it mean that any shape form larger
   than precision can be detected by the quadtree
 */
QuadTree::QuadTree(MultiPolygon& mult, float precision) :
    multiP(mult), precision(precision) {
    float rotationAngle = 360.f / quadsNumber;
    float currentAngle = 0.f;
    trees.reserve(quadsNumber);
    treesOffset.reserve(quadsNumber);
    // Put the shape at the (0,0) point
    Box preEnvelop;
    bg::envelope(multiP, preEnvelop);
    bg::correct(preEnvelop);
    Point preReference = preEnvelop.min_corner();
    // Place the shape into the (0,0) point in order to create the quadTree
    translate<MultiPolygon>(multiP, -preReference.x(), -preReference.y());
    translate<Box>(preEnvelop, -preReference.x(), -preReference.y());
    _totalX = preReference.x();
    _totalY = preReference.y();

    for (unsigned i = 0; i < quadsNumber; i++) {
        // Rotate the MultiPolygon
        MultiPolygon newP;
        bg::strategy::transform::rotate_transformer<bg::degree, float, 2, 2> rotator(
            currentAngle);
        currentAngle += rotationAngle;
        bg::transform(multiP, newP, rotator);
        // Compute the shape Box envelop
        Box envelop;
        bg::envelope(newP, envelop);
        bg::correct(envelop);
        Point reference = envelop.min_corner();
        // Place the shape into the (0,0) point in order to create the quadTree
        translate<MultiPolygon>(newP, -reference.x(), -reference.y());
        translate<Box>(envelop, -reference.x(), -reference.y());
        treesOffset.push_back(reference);
        // We determine maxDepth thanks to the precision
        // If the deaper quadTree width and height need to be smaller than precision
        int maxDepth = 0;
        float width = envelop.max_corner().x();
        float height = envelop.max_corner().y();

        while (width > precision || height > precision) {
            width /= 2;
            height /= 2;
            maxDepth++;
        }

        // Create the bitmap that will be used in order to create the quadTree
        _maxDepth = maxDepth;
        int size = pow(2, maxDepth);
        bmap = new bitmap(mult, size, size);
        // QuadTree size is shape envelop size
        trees.push_back(new InnerQuadTree(envelop.min_corner().x() + _totalX,
                                          envelop.min_corner().y() + _totalY,
                                          envelop.max_corner().x() + _totalX, envelop.max_corner().y() + _totalY,
                                          *bmap, 0, 0, size, 0));
    }
}

/**
 * @brief QuadTree::intersects detect if two QuadTree intersects each other
 * @param q
 * @return
 */
bool QuadTree::intersects(const QuadTree& q) const {
    return trees[currentTree]->intersectsRec(*q.trees[q.currentTree], _totalX, _totalY,
            q._totalX, q._totalY);
}

/**
 * @brief QuadTree::translater translates the QuadTree position
 * @param x
 * @param y
 */
void QuadTree::translater(float x, float y) {
    _totalX += x;
    _totalY += y;
}

/**
 * @brief QuadTree::rotate rotate a QuadTree
 * The rotation is around the (0,0) point by default
 * Better rotation is possible with the possibility to choose the rotation point
 * @param angle rotation angle in degree, stored in radians
 */
void QuadTree::rotater(float angle) {
    unsigned newQuad = (int) round(angle * quadsNumber / 360) % quadsNumber;
    float newAngle = pi * angle / 180.f;
    // Compute the tree position
    float newX = cos(newAngle) * _totalX - sin(newAngle) * _totalY;
    float newY = sin(newAngle) * _totalX + cos(newAngle) * _totalY;
    newX += treesOffset[newQuad].x();
    newY += treesOffset[newQuad].y();
    currentTree = newQuad;
}

/**
 * @brief operator << get informations about the QuadTree for debugging purpose
 * @param s
 * @param q
 */
std::ostream& operator <<(std::ostream& s, const QuadTree& q) {
    s << "Absolute Position : (" << q._totalX << "," << q._totalY << ") " << endl;
    s << " - Trees : " << std::endl;

    for (InnerQuadTree* quad : q.trees)
        s << *quad << std::endl;

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

    for (int i = 0; i < size; i++) {
        std::vector<color_enum> vec = trees[0]->getLine(i, depth);

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

    for (int i = 0; i <= _maxDepth; i++)
        saveTree(filename + "-" + std::to_string(i), i);
}
