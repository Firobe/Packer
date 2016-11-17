#ifndef TYPES__H
#define TYPES__H

#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

namespace bg = boost::geometry;
using Point = bg::model::d2::point_xy<double>; //Defines the Point type
using Polygon = bg::model::polygon<Point>; //Defines the Polygon type
using MultiPolygon = bg::model::multi_polygon<Polygon>; //Defines the MultiPolygon type
using Box = bg::model::box<Point>; //Defines the Box type

#endif
