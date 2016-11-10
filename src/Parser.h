#ifndef PARSER__H
#define PARSER__H

#include <string>
#include <vector>
#include <svgpp/svgpp.hpp>

class Shape;

class Parser {
    private:
        std::string _path;
    public:
        Parser(std::string filePath) : _path(filePath) {}
        void fillShapes(std::vector<Shape>&);
    };


class Context {
    public:
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
        void on_exit_element();
    };
#endif
