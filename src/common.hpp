#ifndef COMMON__H
#define COMMON__H

#include <algorithm>
#include <vector>

#include <boost/geometry/arithmetic/arithmetic.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/algorithms/transform.hpp>
#include <boost/geometry/strategies/transform/matrix_transformers.hpp>
#include <boost/geometry/io/wkt/write.hpp>

namespace bg = boost::geometry;
using Point = bg::model::d2::point_xy<double>; //Defines the Point type
using Ring = bg::model::ring<Point>; //Defines the Ring type
using Polygon = bg::model::polygon<Point>; //Defines the Polygon type
using MultiPolygon = bg::model::multi_polygon<Polygon>; //Defines the MultiPolygon type
using Box = bg::model::box<Point>; //Defines the Box type

/**
 * Utilitary function to check if a vector contains an element.
 */
template<typename T>
bool vectorContains(std::vector<T>& v, T element) {
    return std::find(v.begin(), v.end(), element) != v.end();
}

/**
 * Rotates a boost primitive by a fixed number of
 * degrees (trigonometric).
 */
template<typename T>
void rotate(T& object, double angle) {
    T r;
    //Creates a rotation of <angle> degrees
    bg::strategy::transform::rotate_transformer<bg::degree, double, 2, 2> rotate(angle);
    //Applies that rotation to object and stores result in r
    bg::transform(object, r, rotate);
    //Replaces object with r
    object = r;
}

/**
 * Translates a boost primitive by a fixed vector.
 */
template<typename T>
void translate(T& object, double x, double y) {
    T r;
    bg::strategy::transform::translate_transformer<double, 2, 2>
    translate(x, y);
    bg::transform(object, r, translate);
    object = r;
}

/**
 * Concatenate two vectors
 */
template <typename T>
std::vector<T> operator+(const std::vector<T>& A, const std::vector<T>& B) {
    std::vector<T> AB;
    AB.reserve(A.size() + B.size());
    AB.insert(AB.end(), A.begin(), A.end());
    AB.insert(AB.end(), B.begin(), B.end());
    return AB;
}

/**
 * Stream operator for Boost Geometries
 */
std::ostream& operator<<(std::ostream& os, const Point& p);

/**
 * Relative comparison between floating point numbers
 * DO NOT USE for comparing with ZERO
 * Avoid default epsilon
 */
template<typename T>
static bool floatEqual(T a, T b, T epsilon = std::numeric_limits<T>::epsilon()) {
    T diff = std::fabs(a - b);
    return diff <= epsilon or diff < std::fmax(std::fabs(a), std::fabs(b)) * epsilon;
}

/**
 * Absolute comparison between a floating point number and zero
 * Avoid default epsilon
 */
template<typename T>
static bool floatZero(T a, T epsilon = std::numeric_limits<T>::epsilon()) {
    return std::fabs(a) <= epsilon;
}

#endif
