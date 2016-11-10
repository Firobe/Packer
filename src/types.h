#ifndef TYPES__H
#define TYPES__H

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <rapidxml_ns/rapidxml_ns.hpp>
#include <svgpp/svgpp.hpp>

namespace bg = boost::geometry;
using Point = bg::model::d2::point_xy<double>; //Defines the Point type
using Ring = bg::model::ring<Point>; //Define the Ring type

typedef rapidxml_ns::xml_node<> const* XMLElement;
typedef
boost::mpl::set <
// SVG Structural Elements
svgpp::tag::element::svg,
      svgpp::tag::element::g,
      // SVG Shape Elements
      svgpp::tag::element::path
      >::type ProcessedElements;

#endif
