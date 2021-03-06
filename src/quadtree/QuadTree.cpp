#include <cmath>
#include <iostream>
#include <stdexcept>
#include <fstream>

//TODO: ind the good include for centroid
#include <boost/geometry.hpp>

#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/strategies/cartesian/area_surveyor.hpp>
#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/centroid.hpp>
#include <boost/math/constants/constants.hpp>

#include "bitmap.hpp"
#include "QuadTree.hpp"
#include "InnerQuadTree.hpp"
#include "common.hpp"
#include "Log.hpp"
#include "Shape.hpp"
#include "Matrix.hpp"

using namespace std;

static const float pi = boost::math::constants::pi<float>();

/**
 * @brief QuadTree::copy copy the content of q into this
 * @param q
 */
void QuadTree::copy(const QuadTree& q) {
    treesOffset = q.treesOffset;
    currentTree = q.currentTree;
    _totalX = q._totalX;
    _totalY = q._totalY;
    _maxDepth = q._maxDepth;
    precision = q.precision;
    _area = q._area;
    _centroid = q._centroid;
    _backupX = q._backupX;
    _backupY = q._backupY;
    _backupPos = q._backupPos;
    _oldX = q._oldX;
    _oldY = q._oldY;
}

/**
 * @brief copy constructor
 * @param q
 */
QuadTree::QuadTree(const QuadTree& q) : Shape(q) {
    copy(q);
}

/**
 * @brief assignement operator
 * @param qi
 * @return the quadtree copied
 */
QuadTree& QuadTree::operator=(const Shape& s) {
    const QuadTree& q = dynamic_cast<const QuadTree&>(s);

    if (this != &q) {
        Shape::operator=(q);
        destroy();
        trees = new InnerQuadTree* [quadsNumber];

        for (unsigned i = 0; i < quadsNumber; i++)
            trees[i] = new InnerQuadTree(*(q.trees[i]));

        copy(q);
    }

    return *this;
}

QuadTree& QuadTree::operator=(Shape&& s) {
    QuadTree&& q(dynamic_cast < QuadTree && >(s));

    if (this != &q) {
        Shape::operator=(q);
        destroy();
        trees = q.trees;
        q.trees = nullptr;
        copy(q);
    }

    return *this;
}

void QuadTree::destroy() {
    if (trees != nullptr) {
        for (unsigned i = 0; i < quadsNumber; i++)
            delete trees[i];

        delete[] trees;
    }

    trees = nullptr;
}

/**
 * @brief destructor
 */
QuadTree::~QuadTree() {
    destroy();
}

/**
 * @brief Constructor for Shapes, it just call the MultiPolygon constructor with the Shape's MultiPolygon
 * @param s shape that will be used for the construction
 * @param precision is the precision of the quadtree constructed, it mean that any shape form larger
   than precision can be detected by the quadtree
 */
QuadTree::QuadTree(Shape& s, float precision)
    : trees(nullptr), precision(precision) {
    construct(precision, s);
}

void QuadTree::construct(float precision, Shape& s) {
    trees = nullptr;
    precision = precision;
    Shape::copy(s);
    Matrix m = Shape::getTransMatrix();
    Shape::restore(false);
    construct(precision, s._multiP);
    applyMatrix(m);
}

void QuadTree::construct(float precision, MultiPolygon mult) {
    // mult IS A COPY AND NEEDS TO STAY ONE.
    _area =  bg::area(mult);
    float rotationAngle = 360.f / quadsNumber;
    float currentAngle = 0.f;
    trees = new InnerQuadTree*[quadsNumber];
    treesOffset.clear();
    _centroid.clear();
    treesOffset.reserve(quadsNumber);
    _centroid.reserve(quadsNumber);
    // Put the shape at the (0,0) point
    Box preEnvelop;
    bg::envelope(mult, preEnvelop);
    //bg::correct(preEnvelop);
    Point preReference = preEnvelop.min_corner();
    // Place the shape into the (0,0) point in order to create the quadTree
    ::translate<MultiPolygon>(mult, -preReference.x(), -preReference.y());
    ::translate<Box>(preEnvelop, -preReference.x(), -preReference.y());
    currentTree = 0;
    _oldX = _totalX = preReference.x();
    _oldY = _totalY = preReference.y();

    for (unsigned i = 0; i < quadsNumber; i++) {
        // Rotate the MultiPolygon
        MultiPolygon newP;
        bg::strategy::transform::rotate_transformer<bg::degree, float, 2, 2> rotator(
            currentAngle);
        currentAngle += rotationAngle;
        bg::transform(mult, newP, rotator);
        // Compute the shape Box envelop
        Box envelop;
        bg::envelope(newP, envelop);
        //bg::correct(envelop);
        Point reference = envelop.min_corner();
        // Place the shape into the (0,0) point in order to create the quadTree
        ::translate<MultiPolygon>(newP, -reference.x(), -reference.y());
        ::translate<Box>(envelop, -reference.x(), -reference.y());
        treesOffset.push_back(reference);
        Point p;
        bg::centroid(newP, p);
        _centroid.push_back(p);
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
        bitmap bmap(newP, size, size);
        // QuadTree size is shape envelop size
        trees[i] = new InnerQuadTree(envelop.min_corner().x(),
                                     envelop.min_corner().y(),
                                     envelop.max_corner().x(),
                                     envelop.max_corner().y(),
                                     bmap, 0, 0, size, 0);
    }
}

/**
 * @brief QuadTree::QuadTree is the main constructor of the QuadTree, it will create a bitmap according to the
   desired precision and will use it in order to generate a region QuadTree that represent the given MultiPolygon
 * @param mult is the MultiPolygon that will be transformed into a QuadTree
 * @param precision is the precision of the quadtree constructed, it mean that any shape form larger
   than precision can be detected by the quadtree
 */
QuadTree::QuadTree(MultiPolygon& mult, float precision, unsigned id)
    : Shape(mult, id), trees(nullptr), precision(precision) {
    construct(precision, mult);
}

/**
 * @brief QuadTree::intersects detect if two QuadTree intersects each other
 * @param q
 * @return
 */
bool QuadTree::intersectsWith(const Shape& s) const {
    const QuadTree& q = dynamic_cast<const QuadTree&>(s);
    return trees[currentTree]->intersectsRec(*q.trees[q.currentTree], _totalX, _totalY,
            q._totalX, q._totalY);
}

/**
 * @brief QuadTree::rotate overload of Shape::rotate for quadtrees
 * rotate a QuadTree
 * The rotation is around the (0,0) point by default
 * Better rotation is possible with the possibility to choose the rotation point
 * @param angle rotation angle in degree, stored in radians
 */
void QuadTree::rotate(double degrees) {
    float anglePrecision = 360.0 / quadsNumber;
    float angle = - static_cast<float>(degrees);
    _totalX -= treesOffset[currentTree].x();
    _totalY -= treesOffset[currentTree].y();
    float totalAngle = anglePrecision * currentTree - angle;

    // need a positive angle
    while (totalAngle < 0)
        totalAngle += 360.;

    unsigned newQuad = (int) round(totalAngle / anglePrecision) %
                       quadsNumber;
    float newAngle = pi * angle / 180.f;
    // Compute the tree position
    float newX = cos(newAngle) * _totalX - sin(newAngle) * _totalY;
    float newY = sin(newAngle) * _totalX + cos(newAngle) * _totalY;
    _totalX = newX + treesOffset[newQuad].x();
    _totalY = newY + treesOffset[newQuad].y();
    currentTree = newQuad;
}

/**
 * @brief QuadTree::translate overload of Shape::translate for quadtrees
 * translates the QuadTree position
 * @param x
 * @param y
 */
void QuadTree::translate(double Tx, double Ty) {
    _totalX += static_cast<float>(Tx);
    _totalY += static_cast<float>(Ty);
}

void QuadTree::envelope(Box& b) const {
    b = {{_totalX, _totalY}, {_totalX + trees[currentTree]->x2, _totalY + trees[currentTree]->y2}};
}

int QuadTree::area() const {
    return _area;
}

Point QuadTree::centroid() const {
    return Point{_totalX + _centroid[currentTree].x(),
                 _totalY + _centroid[currentTree].y()};
}

/**
 * @brief operator << get informations about the QuadTree for debugging purpose
 * @param s
 * @param q
 */
std::ostream& operator <<(std::ostream& s, const QuadTree& q) {
    s << "QuadTree n° : " << q.getIdentifier() << endl;
    s << "Absolute Position : (" << q._totalX << "," << q._totalY << ") " << endl;
    s << "Current tree (angle) : " << q.currentTree << "(" << 30 * q.currentTree << ")" <<
      endl;
    s << "Trees : " << std::endl;

    for (unsigned i = 0; i < q.quadsNumber; i++)
        s << " - " << *(q.trees[i]) << "    " << bg::wkt(q.treesOffset[i]) << endl;

    return s;
}

/**
 * @brief QuadTree::saveTree save the depth QuadTree in the PGM format
 * @param filename base name of the file, the full name will be : filename.pbm
 * @param depth
 */
void QuadTree::saveTree(std::string filename, int depth) {
    for (unsigned k = 0; k < quadsNumber; k++) {
        int size = pow(2, depth);
        ofstream file;
        file.open(to_string(k) + filename + ".pgm");
        file << "P2" << endl;
        file << size << " " << size << endl;
        file << "2" << endl;

        for (int i = 0; i < size; i++) {
            std::vector<color_enum> vec = trees[k]->getLine(i, depth);

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
}

/**
 * @brief QuadTree::saveTree save the any QuadTree's depth in the PGM format
 * @param filename base name of the file, the full name will be : filename-depth.pbm
 */
void QuadTree::saveTree(std::string filename) {
    for (int i = 0; i <= _maxDepth; i++)
        saveTree(filename + "-" + std::to_string(i), i);
}

void QuadTree::savePos() {
    _backupX = _totalX;
    _backupY = _totalY;
    _backupPos = currentTree;
}

void QuadTree::restorePos() {
    _totalX = _backupX;
    _totalY = _backupY;
    currentTree = _backupPos;
}

array<double, 6> QuadTree::getTransMatrix() const {
    std::array<double, 6> result;
    float angle = (2 * currentTree * pi) / quadsNumber;
    float c = cos(angle);
    float s = sin(angle);
    result[0] = c;
    result[1] = s;
    result[2] = -s;
    result[3] = c;
    float c2 = cos(-angle);
    float s2 = sin(-angle);
    float newX = c2 * _oldX - s2 * _oldY;
    float newY = s2 * _oldX + c2 * _oldY;
    result[4] = _totalX - newX - treesOffset[currentTree].x();
    result[5] = _totalY - newY - treesOffset[currentTree].y();
    return result;
}

void applyTransMatrix(MultiPolygon& multiP, const array<double, 6>& transformMatrix) {
    double theta = atan2(transformMatrix[1], transformMatrix[3]);
    ::rotate<MultiPolygon>(multiP, theta * 180. / pi);
    ::translate<MultiPolygon>(multiP, transformMatrix[4], transformMatrix[5]);
}

void QuadTree::mergeWith(const Shape& s) {
    // First apply the quadTree transformation to both shapes
    array<double, 6> transformMatrix;
    transformMatrix = getTransMatrix();
    applyTransMatrix(_multiP, transformMatrix);
    //IMPORTANT : don't modify s multiP
    MultiPolygon mult = s._multiP;
    const QuadTree& q = dynamic_cast<const QuadTree&>(s);
    transformMatrix = q.getTransMatrix();
    applyTransMatrix(mult, transformMatrix);
    mergeMultiP(_multiP, mult);
    destroy();
    construct(precision, *this);
}

bool QuadTree::intersectsWith(const Ring& r) const {
    static bool warning = false;

    if (!warning) {
        cerr << "instersectWith(Ring) : THIS ALGORITHM IS NOT IMPLEMENTED FOR QUADTREES" << endl
             <<
             "                       AND WILL USE THE MULTIPOLYGON REPRESENTATION TO COMPUTE THE DESIRED RESULT"
             << endl;
        warning = true;
    }

    MultiPolygon mult = _multiP;
    applyTransMatrix(mult, getTransMatrix());
    return bg::overlaps(mult, r);
}

void QuadTree::convexHull(Polygon& p) const {
    static bool warning = false;

    if (!warning) {
        cerr << "convexHull(Polygon) : THIS ALGORITHM IS NOT IMPLEMENTED FOR QUADTREES" << endl <<
             "                      AND WILL USE THE MULTIPOLYGON REPRESENTATION TO COMPUTE THE DESIRED RESULT"
             << endl;
        warning = true;
    }

    MultiPolygon mult = _multiP;
    applyTransMatrix(mult, getTransMatrix());
    bg::convex_hull(mult, p);
}

void QuadTree::bufferize(double d) {
    static bool warning = false;

    if (!warning) {
        cerr << "bufferize(double) : THIS ALGORITHM IS NOT IMPLEMENTED FOR QUADTREES" << endl <<
             "                    AND WILL USE THE MULTIPOLYGON REPRESENTATION TO COMPUTE THE DESIRED RESULT"
             << endl <<
             "                    WILL REGENERATE QUADTREE, SPEED OF THE PROGRAM WILL SLOW DOWN IF THIS METHOD IS USE OFTEN"
             << endl;
        warning = true;
    }

    applyTransMatrix(_multiP, getTransMatrix());
    Shape::bufferize(d);
    destroy();
    construct(precision, _multiP);
}
