#ifndef TYPES__H
#define TYPES__H

#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <algorithm>
#include <vector>

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

#endif
