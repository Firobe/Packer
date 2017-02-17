#ifndef CLOSE_ENOUGH__HPP
#define CLOSE_ENOUGH__HPP

#include <boost/spirit/include/qi.hpp>
#include <string>

#include "Shape.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

/*
Grammar definition (double_ and int_ already defined)
=====================================================
    string_ 			[^",()= ]
    value = 			double_
                        | string_
    parameter =			string_ '=' value
    parameter_list =	parameter | parameter ',' parameter_list | eps
    transformer = 		"SimpleTransformer"
                        | "Reset"
    solver = 			"ScanlineSolver"
                        | "TheSkyIsTheLimitSolver"
    function_ =			transformer
                        | solver
	instruction =		function_ '(' parameter_list ')' ';'
    instruction_list =	(instruction)*
    block =				instruction
                        | "BEGIN" instruction_list "END"
    big_block = 		block
                        | "DO" int_ "TIMES" block
    start = 			(big_block)*

Language usage
===============
An instruction have the following pattern : function(param1=value1, param2=value2, ...);
Instructions must be followed by a semicolon
Multiple-instruction blocks are between a BEGIN and a END
Instruction blocks can be executed multiple times using DO X TIMES <block>

Example
=========
DO 2 TIMES
BEGIN
SimpleTransformer(criteria=intersection);
SimpleTransformer(criteria=box);
END
ScanlineSolver();
*/

/**
 * Struct used for parameter values
 * Can be a double or a string. isNum is set accordingly
 */
struct Value {
    bool isNum;
    double number;
    std::string str;
};

/**
 * Struct used for functions  calls.
 * Every parameter has a name and a value : name=value
 */
struct Parameter {
    std::string name;
    Value value;
};


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
