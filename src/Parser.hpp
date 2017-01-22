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

//Maximal deviation in bezier interpolation
//A smaller value means more points but shapes will be closer to each other
#define BEZIER_TOLERANCE 0.5

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
    Point& _docDim;
    Matrix _currentMatrix;
    int _toApply; //Number of recently created rings
public:
    static std::vector<Shape> Parse(std::string, std::vector<std::string>&, Point&);

    Parser(std::vector<std::string>& i, Point& d) :
        _ids(i), _groupStack(-1), _docDim(d),
        _currentMatrix(1, 0, 0, 1, 0, 0)
    {}
    std::vector<Shape> getShapes() {
        return _shapes;
    }

    ///SVG++ Methods
    //void set(svgpp::tag::attribute::id, std::string pId);
    void transform_matrix(const boost::array<double, 6>& matrix);
    void set(svgpp::tag::attribute::id, const boost::iterator_range<const char*> pId);
    void set(svgpp::tag::attribute::width, double width);
    void set(svgpp::tag::attribute::height, double height);
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
                                  XMLAttributesIterator const&,
                                  AttributeName const&,
                                  svgpp::tag::source::css) {
        return true;
    }
    template<class XMLAttributesIterator, class AttributeName>
    static bool unknown_attribute(context_type&,
                                  XMLAttributesIterator const&,
                                  AttributeName const&,
                                  BOOST_SCOPED_ENUM(svgpp::detail::namespace_id),
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

/**
 * Select the additionnal attributes that will
 * be processed by our parser
 */
using CustomAttributes =
    boost::mpl::set <
    svgpp::tag::attribute::height,
    svgpp::tag::attribute::id,
    svgpp::tag::attribute::width,
    svgpp::tag::attribute::transform
    >::type;

/**
 * Merge default processed attributes and custom
 * ones, using beautiful functional meta-programming.
 */
using ProcessedAttributes =
    boost::mpl::fold <
    CustomAttributes,
    svgpp::traits::shapes_attributes_by_element,
    boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
    >::type;

#endif
