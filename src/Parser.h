#ifndef PARSER__H
#define PARSER__H

#include <string>
#include <vector>
#include "types.h"

class Shape;

class Parser {
private:
    std::vector<Shape> _shapes;
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
    void path_quadratic_bezier_to(
        double x1, double y1,
        double x, double y,
        svgpp::tag::coordinate::absolute);
    void path_elliptical_arc_to(
        double rx, double ry, double x_axis_rotation,
        bool large_arc_flag, bool sweep_flag,
        double x, double y,
        svgpp::tag::coordinate::absolute);
    void path_close_subpath();
    void path_exit();

    void on_enter_element(svgpp::tag::element::any);
    void on_exit_element();
};

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
                                  auto namespace_id, ///THIS WILL ONLY COMPILE WITH -std=c++14
                                  svgpp::tag::source::attribute) {
        return true;
    }
};
#endif
