#ifndef TYPES__H
#define TYPES__H

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

namespace bg = boost::geometry;
using Point = bg::model::d2::point_xy<double>; //Defines the Point type
using Ring = bg::model::ring<Point>; //Define the Ring type

#endif
