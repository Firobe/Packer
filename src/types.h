#ifndef TYPES__H
#define TYPES__H

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <rapidxml_ns/rapidxml_ns.hpp>
#include <svgpp/svgpp.hpp>

namespace bg = boost::geometry;
using Point = bg::model::d2::point_xy<double>; //Defines the Point type
using Ring = bg::model::ring<Point>; //Defines the Ring type
using XMLElement = rapidxml_ns::xml_node<> const*; //Defines the XMLElement type
using Box = bg::model::box<Point>; //Defines the Box type

/**
 * Select the tags that will be processed
 * by our parser
 */
using ProcessedElements =
    boost::mpl::set <
    // SVG Structural Elements
    svgpp::tag::element::svg,
    svgpp::tag::element::g,
    // SVG Shape Elements
    svgpp::tag::element::path,
    svgpp::tag::element::rect,
    svgpp::tag::element::ellipse,
    svgpp::tag::element::line,
    svgpp::tag::element::polygon,
    svgpp::tag::element::polyline,
    svgpp::tag::element::circle
    //Text and other things not handled
    >::type;

#endif
