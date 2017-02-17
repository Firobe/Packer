#ifndef CLOSE_ENOUGH__HPP
#define CLOSE_ENOUGH__HPP

#include <string>
#include <iostream>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix.hpp>
#include <boost/function.hpp>

#include "Shape.hpp"
#include "common.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

/**
 *
 * CloseEnough scripting language
 * ==============================
 * A very simple DSL to facilitate the combination of
 * the different packing algorithms without having to rebuild.
 *
 * Features : function calls with parameters, ability to repeat instruction blocks
 *
 * Grammar definition (double_ and int_ already defined)
 * =====================================================
 *     string_ 			[^",()= ]
 *     value = 			double_
 *                         | string_
 *     parameter =			string_ '=' value
 *     parameter_list =	parameter | parameter ',' parameter_list | epsilon
 *     transformer = 		"SimpleTransformer"
 *                         | "Reset"
 *     solver = 			"ScanlineSolver"
 *                         | "TheSkyIsTheLimitSolver"
 *     function_ =			transformer
 *                         | solver
 * 		instruction =		function_ '(' parameter_list ')' ';'
 *     instruction_list =	(instruction)*
 *     block =				instruction
 *                         | "BEGIN" instruction_list "END"
 *     big_block = 		block
 *                         | "DO" int_ "TIMES" block
 *     start = 			(big_block)*
 *
 * Language usage
 * ===============
 * An instruction have the following pattern : function(param1=value1, param2=value2, ...);
 * Instructions must be followed by a semicolon
 * Multiple-instruction blocks are between a BEGIN and a END
 * Instruction blocks can be executed multiple times using DO X TIMES <block>
 *
 * Example
 * =========
 * DO 2 TIMES
 * BEGIN
 * SimpleTransformer(criteria=intersection);
 * SimpleTransformer(criteria=box);
 * END
 * ScanlineSolver();
 */

/**
 * Get the value of the parameter named <key>
 * in a list of parameters and checks wether it
 * is of the required type.
 */
template<typename Required>
bool getParameter(std::vector<Parameter> p, std::string key, Required& value) {
    Value v;
    bool b = getParameter(p, key, v);

    if (b and v.type() == typeid(Required))
        value = boost::get<Required>(v);
    else return false;

    return true;
}

/**
 * Value specialization of getParameter
 */
template<>
bool getParameter(std::vector<Parameter> p, std::string key, Value& value);

/**
 * Struct used for functions
 * Every function is either a transformer or a solver
 * and has a name.
 */
enum funcCategory { FUNC_TRANSFORMER, FUNC_SOLVER };
struct Function {
    std::string name;
    funcCategory cat;
};

/**
 * Struct used for function calls
 * A call is a function and the given parameters
 * The operator() can be used to actually make the call
 */
struct Call {
    Function func;
    std::vector<Parameter> params;
    void operator()(std::vector<Shape>& shapes);
};

/**
 * Base template for a registry class
 * used as a hash table to instanciate child classes from a base classes
 * with only their name.
 * The child classes must be registered, and then they can be instanciated.
 *
 * Here every instanciated class must have a constructor taking a vector of shapes
 * and a vector of parameters
 */
template<typename BaseClass>
class Registry {
    std::map<std::string, boost::function<BaseClass* (std::vector<Shape>&, std::vector<Parameter>)>>
            _fact;
public:
    //Usage : reg<ClassToRegister>();
    template<typename DerivedClass>
    void reg(std::string name) {
        _fact[name] = phoenix::new_<DerivedClass>(phoenix::placeholders::arg1,
                      phoenix::placeholders::arg2);
    }
    //Usage : instanciate("className", shapes, parameters); Returns a BaseClass pointer
    BaseClass* instanciate(std::string name, std::vector<Shape>& s,
                           std::vector<Parameter> p) {
        return _fact.at(name)(s, p);
    }
};

/**
 * The struct used to define the
 * grammar of the language.
 * Attributes are the rule of the grammar, which are defined
 * in the constructor
 */
struct CE_Parser : qi::grammar<std::string::iterator, void(), ascii::space_type> {

    //The second template parameter is the type of the rule's attribute
    qi::rule<std::string::iterator, Value, ascii::space_type> 						value;
    qi::rule<std::string::iterator, std::string(), ascii::space_type> 				string_;
    qi::rule<std::string::iterator, Parameter(), ascii::space_type> 				parameter;
    qi::rule<std::string::iterator, std::vector<Parameter>(), ascii::space_type>
    parameter_list;
    qi::rule<std::string::iterator, std::string(), ascii::space_type> 				transformer;
    qi::rule<std::string::iterator, std::string(), ascii::space_type> 				solver;
    qi::rule<std::string::iterator, Function(), ascii::space_type> 					function_;
    qi::rule<std::string::iterator, Call(), ascii::space_type> 						instruction;
    qi::rule<std::string::iterator, std::vector<Call>(), ascii::space_type>
    instruction_list;
    qi::rule<std::string::iterator, std::vector<Call>(), ascii::space_type> 		block;
    qi::rule<std::string::iterator, void(), ascii::space_type> 						big_block;
    qi::rule<std::string::iterator, void(), ascii::space_type> 						start;

    CE_Parser(std::vector<Shape>& s);
};

#endif
