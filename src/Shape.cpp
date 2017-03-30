#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include <boost/geometry/algorithms/buffer.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/strategies/cartesian/area_surveyor.hpp>
#include <boost/geometry/algorithms/covered_by.hpp>
#include <boost/geometry/strategies/agnostic/point_in_poly_winding.hpp>
#include <boost/geometry/strategies/agnostic/relate.hpp>
#include <boost/geometry/io/svg/svg_mapper.hpp>
#include <boost/geometry/algorithms/num_points.hpp>
#include <boost/geometry/algorithms/intersects.hpp>
#include <boost/geometry/algorithms/centroid.hpp>
#include <boost/geometry/algorithms/convex_hull.hpp>
#include <boost/math/constants/constants.hpp>

#include "Shape.hpp"
#include "Log.hpp"
#include "Parser.hpp"
#include "Interpolator.hpp"

using namespace std;

/**
 * Defines a comparator for Rings
 */
inline bool ringLess(const Ring& a, const Ring& b) {
    return bg::area(a) < bg::area(b);
}

/**
 * Point stream operator
 */
std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << bg::wkt(p);
    return os;
}

/**
 * @brief mergeMultiP Stores all of B's polygons into A
 * @param A
 * @param B
 */
void mergeMultiP(MultiPolygon& A, const MultiPolygon& B) {
    for (auto& b : B)
        A.push_back(b);
}

/**
 * @brief Returns [a, b, c, d, e, f] corresponding to the 3*3 matrix :
 * a c e
 * b d f
 * 0 0 1
 * being the transformation matrix of the shape relatively to its
 * starting position.
 */
array<double, 6> Shape::getTransMatrix() const {
    Point newP1 = _multiP[0].outer()[_indexP1];
    Point newP2 = _multiP[0].outer()[_indexP2];
    double c, s, x1, y1, x2, y2, n1, n2;
    //Normalize vectors (x1, y1), (x2, y2)
    n1 = bg::distance(_oldP1, _oldP2);
    n2 = bg::distance(newP1, newP2);

    if (!floatEqual(n1, n2, 10e-6))
        throw runtime_error("Point order was modified or the shape was scaled");

    x1 = (_oldP2.x() - _oldP1.x()) / n1;
    y1 = (_oldP2.y() - _oldP1.y()) / n1;
    x2 = (newP2.x() - newP1.x()) / n2;
    y2 = (newP2.y() - newP1.y()) / n2;
    //Computing cos & sin with dot products
    c = x1 * x2 + y1 * y2;
    s = x1 * y2 - x2 * y1;
    //Resulting matrix corresponding to the following operations:
    //Translate to origin, rotate, translate to new position
    array<double, 6> result;
    result[0] = c;
    result[1] = -s;
    result[2] = s;
    result[3] = c;
    result[4] = - _oldP1.x() * c + _oldP1.y() * s + newP1.x();
    result[5] = - _oldP1.x() * s - _oldP1.y() * c + newP1.y();
    return result;
}

/**
 * @brief Initializes the boost model with a set of points.
 * (should be a closed path)
 *
 * Also initializes _oldP1, P2, _indexP1, P2 which should
 * be two points from the shape as for as possible from each other.
 * (will be used to compute transformation matrix)
 * @param rings vector of rings to be processed
 */
void Shape::fillShape(vector<Ring>& rings) {
    LOG(debug) << "Received " << rings.size() << " Rings for a Shape" << endl;
    //Sorting the rings by increasing area
    sort(rings.begin(), rings.end(), ringLess);

    for (unsigned i = 0 ; i < rings.size() ; ++i) {
        bool covered = false;

        //We can search only for rings after i thanks to the ring sorting
        for (unsigned j = rings.size() - 1 ; j > i ; --j) {
            if (bg::covered_by(rings[i], rings[j])) {
                covered = true;
                break;
            }
        }

        if (!covered) { //The ring i not covered by anyone
            LOG(debug) << "Ring " << i << " is an outer ring" << endl;
            //Add a new polygon to the MultiPolygon
            _multiP.resize(_multiP.size() + 1);
            //The ring is the outer ring of the new polygon
            _multiP.back().outer() = rings[i];

            //Discover all the holes of i
            //We can search only for rings before i thanks to the ring sorting
            for (unsigned k = 0 ; k < i ; ++k)
                if (bg::covered_by(rings[k], rings[i])) {
                    LOG(debug) << "  -> Ring " << k << " is one if its holes" << endl;
                    //Add an inner ring to our polygon
                    _multiP.back().inners().resize(_multiP.back().inners().size() + 1);
                    //k is that inner ring
                    _multiP.back().inners().back() = rings[k];
                }
        }
    }

    setOld();
}

/**
 * @brief Change shapes by adding a buffer
 * (of <buffer>+BEZIER_TOLERANCE px) around each of them.
 * _oldP1, P2, indexP1, P2 are also recalculated according to the new multiPolygon
 * @param buffer Size of the buffer in pixels
 */
void Shape::bufferize(double buffer) {
    /*
     * Buffering at least the interpolation maximal deviation
     * With this we guarantee that there is NO intersection between shapes (or at least it should)
     * due to the interpolation error.
     */
    buffer += 2 * BEZIER_TOLERANCE;
    // Declare strategies
    static bg::strategy::buffer::distance_symmetric<double> distance_strategy(buffer);
    static bg::strategy::buffer::join_miter join_strategy(
        2.); //Points will be located to at most 2 * buffer
    static bg::strategy::buffer::end_flat end_strategy;
    static bg::strategy::buffer::point_square circle_strategy;
    static bg::strategy::buffer::side_straight side_strategy;
    MultiPolygon result;
    // Create the buffer of a multi polygon
    bg::buffer(_multiP, result,
               distance_strategy, side_strategy,
               join_strategy, end_strategy, circle_strategy);

    for (unsigned i = 0 ; i < result.size() ; ++i) {
        result[i].inners().resize(_multiP[i].inners().size());

        for (unsigned j = 0 ; j < result[i].inners().size() ; ++j)
            result[i].inners()[j] = _multiP[i].inners()[j];
    }

    _multiP = result;
    LOG(debug) << "Number of points : " << bg::num_points(_multiP) << endl;
    setOld();
}

/**
 * @brief Store points for future transformation reference.
 * Find two points that maximize distance between them.
 * The first point is currently always at index 0.
 * Points are only concidered in the first Polygon of the shape
 */
void Shape::setOld() {
    _indexP1 = 0;
    _indexP2 = 0;

    for (unsigned i = 1 ; i < _multiP[0].outer().size() ; ++i)
        if (bg::distance(_multiP[0].outer()[_indexP1], _multiP[0].outer()[i]) >
                bg::distance(_multiP[0].outer()[_indexP1], _multiP[0].outer()[_indexP2]))
            _indexP2 = i;

    _oldP1 = _multiP[0].outer()[_indexP1];
    _oldP2 = _multiP[0].outer()[_indexP2];
}

/**
 * @brief Merge another Shape into the current one
 * @param s another Shape
 */
void Shape::mergeWith(const Shape& s) {
    mergeMultiP(_multiP, s._multiP);
}

/**
 * @brief Rotate a shape by <angle>
 * @param angle In degrees
 */
void Shape::rotate(double angle) {
    ::rotate<MultiPolygon> (_multiP, angle);
}

/**
 * @brief Translate a shape by (x, y)
 * @param x
 * @param y
 */
void Shape::translate(double x, double y) {
    ::translate<MultiPolygon> (_multiP, x, y);
}

/**
 * @brief Compute the rectangular envelope
 * as a Box, in place
 * @param b Box where the result will be stored
 */
void Shape::envelope(Box& b) const {
    bg::envelope(_multiP, b);
}

/**
 * @brief Returns the area of the Shape
 */
int Shape::area() const {
    return bg::area(_multiP);
}

/**
 * @brief Returns the centroid of the Shape
 */
Point Shape::centroid() const {
    Point p;
    bg::centroid(_multiP, p);
    return p;
}

/**
 * @brief Check if the Shape intersects with another
 * @param s Another shape
 */
bool Shape::intersectsWith(const Shape& s) const {
    return bg::intersects(_multiP, s._multiP);
}

/**
 * @brief Check if the Shape intersects with a Ring
 * @param s A ring
 */
bool Shape::intersectsWith(const Ring& s) const {
    return bg::intersects(_multiP, s);
}

/**
 * @brief Generates the convex hull of the Shape into a polygon
 * @param p Polygon where the result will be stored
 */
void Shape::convexHull(Polygon& p) const {
    bg::convex_hull(_multiP, p);
}

/**
 * @brief Output function using the svg output methods of BOOST.
 * Should be used for debug only.
 * Outputs what the solver actually sees.
 */
string Shape::debugOutputSVG() const {
    stringstream ret;
    bg::svg_mapper <Point> mapper(ret, 800, 800);
    mapper.add(_multiP);
    mapper.map(_multiP, "fill:rgb(" + to_string(rand() % 256) + "," +
               to_string(rand() % 256) + "," + to_string(rand() % 256) + ")");
    LOG(info) << "Debug SVG generated" << endl;
    return ret.str() + "</svg>";
}

/**
 * @brief Rotates object so that its retangular bounding box
 * will be of minimal area
 * Explores a range of 90 degrees with a fixed step
 * @param object Shape to be processed
 */
void rotateToBestAngle(Shape& object) {
    const double ANGLE_MAX = 90.0;
    const double ANGLE_STEP = 0.2;
    double bestAngle, currAngle;
    double bestArea;
    Box currBox;
    object.envelope(currBox);
    bestAngle = 0.0;
    bestArea = bg::area(currBox);
    currAngle = 0.0;

    while (currAngle <= ANGLE_MAX) {
        double currArea;
        object.rotate(ANGLE_STEP);
        object.envelope(currBox);
        currArea = bg::area(currBox);

        if (currArea < bestArea) {
            bestArea = currArea;
            bestAngle = currAngle;
        }

        currAngle += ANGLE_STEP;
    }

    object.rotate(bestAngle - currAngle);
    object.envelope(currBox);

    // To have height > width
    if (abs(currBox.max_corner().y() - currBox.min_corner().y()) < abs(
                currBox.max_corner().x() - currBox.min_corner().x()))
        object.rotate(90);
}

const std::string& Shape::getIdentifier() const {
    return Parser::id(_id);
}

const std::string& Shape::getOut() const {
    return _out;
}

Point Shape::getOldP1() const {
    return _oldP1;
}

Point Shape::getOldP2() const {
    return _oldP2;
}

unsigned Shape::getIndexP1() const {
    return _indexP1;
}

unsigned Shape::getIndexP2() const {
    return _indexP2;
}

/**
 * @brief Appends a string to _out
 * @param s String to be appended
 */
void Shape::appendOut(const std::string& s) {
    _out += s;
}

/**
 * @brief Returns the number of polygons
 */
unsigned Shape::polyNumber() const {
    return _multiP.size();
}

/**
 * @brief Returns a reference on the nth Polygon
 * @param n
 */
const Polygon& Shape::getNthPoly(unsigned i) const {
    return _multiP[i];
}

unsigned Shape::getID() const {
    return _id;
}

/**
 * @brief Add the nth polygon of s into the current shape
 * @param s A shape
 * @param n
 */
void Shape::addNthPolygon(const Shape& s, unsigned n) {
    _multiP.push_back(s._multiP[n]);
}

/**
 * @brief Reserve space into the MultiPolygon
 * @param nbPoly Number of polygons to reserve
 */
void Shape::reserve(int nbPoly) {
    _multiP.reserve(nbPoly);
}

void Shape::copy(const Shape& s) {
    _multiP = s._multiP;
    _oldP1 = s._oldP1;
    _oldP2 = s._oldP2;
    _indexP1 = s._indexP1;
    _indexP2 = s._indexP2;
    _id = s._id;
    _out = s._out;
}

void Shape::restore() {
    Matrix m = getTransMatrix();
    applyMatrix(m, true);
}

void Shape::applyMatrix(Matrix& transM, bool inverse) {
    static const double pi = boost::math::constants::pi<double>();
    double theta = atan2(transM[1], transM[3]);

    /*
    if(fromTrans){
    	transM[1] *= -1; //Fuck Inkscape
    	transM[2] *= -1;
    }
    */
    if (inverse) {
        translate(-transM[4], -transM[5]);
        rotate(-theta * 180. / pi);
    }
    else {
        rotate(theta * 180. / pi);
        translate(transM[4], transM[5]);
    }
}
