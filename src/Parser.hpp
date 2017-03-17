#ifndef PARSER__H
#define PARSER__H

#include <string>
#include <vector>

#include <rapidxml_ns/rapidxml_ns.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/traits/attribute_groups.hpp>
#include <svgpp/policy/error.hpp>

#include <boost/mpl/set.hpp>

#include "common.hpp"
#include "Matrix.hpp"
#include "Shape.hpp"

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
    /**
     * The final vector of shapes that ought to be returned to the main
     */
    std::vector<Shape> _shapes;

    /**
     * The rings parsed at a certain moment of the document traversal.
     * Is cleared each time rings are sent into a shape.
     */
    std::vector<Ring> _rings;

    /**
     * The points parsed at a certain moment of the document traversal.
     * Is cleared each time points are sent into a ring.
     */
    std::vector<Point> _points;

    /**
     * A stack of IDS corresponding to the IDS parsed at a certain moment.
     * Is popped each time we quit an element.
     */
    std::stack<std::string> _idStack;

    /**
     * A vector of IDs that are meant to be packed. Used for filtering which shapes
     * are returned to main. May be updated during the process.
     */
    std::vector<std::string>& _ids;

    /**
     * Used to known if we are in a group at a certain moment and to identify when
     * closing tags are closing a group.
     * > 0 when in a group
     * = 0 when closing a group
     * < 0 else
     */
    int _groupStack;
    Matrix _currentMatrix;
    int _toApply; //Number of recently created rings
    static std::vector<std::string> _identifiers; //Contains all unique shape identifiers
    static Point _binDims;

    Parser(std::vector<std::string>& i) :
        _ids(i), _groupStack(-1),
        _currentMatrix(1, 0, 0, 1, 0, 0), _toApply(0) {
    }
public:
    static std::vector<Shape> Parse(const std::string&, std::vector<std::string>&);
    static const std::string& id(unsigned i) {
        return _identifiers[i];
    }
    static const Point& getDims() {
        return _binDims;
    }
    static void setDims(const Point& p) {
        _binDims = p;
    }

    std::vector<Shape> getShapes() {
        return _shapes;
    }

    ///SVG++ Methods
    //void set(svgpp::tag::attribute::id, std::string pId);
    void transform_matrix(const boost::array<double, 6>& matrix);
    void set(svgpp::tag::attribute::id, const boost::iterator_range<const char*> pId);
    void set(svgpp::tag::attribute::width, double width);
    void set(svgpp::tag::attribute::height, double height);
    void set(svgpp::tag::attribute::viewBox, double, double, double, double);
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

#endif
