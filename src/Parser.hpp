#ifndef PARSER__H
#define PARSER__H

#include <string>
#include <vector>
#include <rapidxml_ns/rapidxml_ns.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/policy/error.hpp>
#include <boost/mpl/set.hpp>

#include "types.hpp"

using XMLElement = rapidxml_ns::xml_node<> const* ; //Defines the XMLElement type
class Shape;

/**
 * Parser class. Used as a Context for SVG++ (thus
 * implements several SVG++ methods)
 *
 * http://svgpp.org/lesson01.html#id1
 *
 * The static method starts the parsing of a file,
 * filling _shapes, then returns the parsed (and
 * interpolated) shapes.
 */
class Parser {
private:
    std::vector<Shape> _shapes;
    std::vector<Point> _points;
public:
    static std::vector<Shape> Parse(std::string);
    std::vector<Shape> getShapes() {
        return _shapes;
    }

    ///SVG++ Methods
    void path_move_to(double x, double y, svgpp::tag::coordinate::absolute);
    void path_line_to(double x, double y, svgpp::tag::coordinate::absolute);
    void path_cubic_bezier_to(
        double x1, double y1,
        double x2, double y2,
        double x, double y,
        svgpp::tag::coordinate::absolute);
    void path_close_subpath();
    void path_exit();

    void on_enter_element(svgpp::tag::element::any);
    void on_enter_element(svgpp::tag::element::g);
    void on_exit_element();
};

/**
 * This enables the parser to ignore any unknown attribute
 * or CSS property and thus avoid a fatal error on
 * files not SVG 1.1 compliant.
 */
struct IgnoreError : svgpp::policy::error::raise_exception<Parser> {
    template<class XMLAttributesIterator, class AttributeName>
    static bool unknown_attribute(context_type&,
                                  XMLAttributesIterator const& attribute,
                                  AttributeName const& name,
                                  svgpp::tag::source::css) {
        return true;
    }
    template<class XMLAttributesIterator, class AttributeName>
    static bool unknown_attribute(context_type&,
                                  XMLAttributesIterator const& attribute,
                                  AttributeName const& name,
                                  BOOST_SCOPED_ENUM(svgpp::detail::namespace_id) namespace_id,
                                  svgpp::tag::source::attribute) {
        return true;
    }
};

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
