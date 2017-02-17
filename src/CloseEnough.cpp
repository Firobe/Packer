#include <iostream>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include "CloseEnough.hpp"
#include "Merger.hpp"
#include "SimpleTransformer.hpp"
#include "solver/box/Scanline.hpp"
#include "solver/box/TheSkyIsTheLimit.hpp"

using qi::double_;
using qi::int_;
using qi::_1;
using qi::_2;
using qi::_3;
using qi::_4;
using qi::on_error;
using qi::fail;
using qi::eps;
using qi::_val;
using qi::lit;
using qi::lexeme;
using ascii::space;
using ascii::char_;
using phoenix::bind;
using phoenix::val;
using phoenix::construct;
using phoenix::push_back;

using namespace std;

/**
 * Stream operator for Value
 */
ostream& operator<< (ostream& os, const Value& c) {
    os << (c.isNum ? to_string(c.number) : c.str);
    return os;
}

/**
 * Stream operator for Call
 */
ostream& operator<< (ostream& os, const Call& call) {
    os << "Calling " << call.func.name << " (" << call.func.cat << ")" << endl;
    os << "with parameters ";

    for (auto && i : call.params)
        os << "(" << i.name << ", " << i.value << ") ";

    os << endl;
    return os;
}

/**
 * Helper functions to create structs
 * with the appropriate parameters
 */
Value makeNumberValue(double d) {
    return {true, d, ""};
}
Value makeStringValue(string s) {
    return {false, 0., s};
}
Parameter makeParameter(string s, Value v) {
    return {s, v};
}
Function makeTransFunction(string s) {
    return {s, FUNC_TRANSFORMER};
}
Function makeSolverFunction(string s) {
    return {s, FUNC_SOLVER};
}
Call makeCall(Function f, vector<Parameter> p) {
    return {f, p};
}

/**
 * Actually executes every call of every block n times
 */
void callEverything(vector<Call> block, int n, vector<Shape>* shapes) {
    for (int i = 0 ; i < n ; ++i)
        for (auto && j : block)
            j(*shapes);
}

/**
 * Given a list of parameters and a key string,
 * returns if such a key is found and fills value with its value
 * if found
 * (string overload)
 */
bool getParameter(vector<Parameter> p, string key, string& value) {
    auto it = find_if(p.begin(), p.end(), [key](const Parameter & p) {
        return p.name == key;
    });

    if (it == p.end() || it->value.isNum)
        return false;

    value = it->value.str;
    return true;
}

/**
 * Idem, double overload
 */
bool getParameter(vector<Parameter> p, string key, double& value) {
    auto it = find_if(p.begin(), p.end(), [key](const Parameter & p) {
        return p.name == key;
    });

    if (it == p.end() || !it->value.isNum)
        return false;

    value = it->value.number;
    return true;
}

/**
 * Given a Call struct, make the appropriate
 * function calls with appropriate parameters
 */
void Call::operator()(vector<Shape>& shapes) {
    static Merger merge(shapes);

    if (func.cat == FUNC_TRANSFORMER) {
        vector<vector<unsigned>> transformed;

        if (func.name == "SimpleTransformer") {
            string criteria, mergeP;

            if (!getParameter(params, "criteria", criteria))
                throw runtime_error("Please specify a criteria for the transformer");

            if (!getParameter(params, "merge", mergeP))
                mergeP = "true";

            if (criteria == "intersection") {
                SimpleTransformer<IntersectionCriteria> st(shapes);
                transformed = st.transform();
            }
            else if (criteria == "box") {
                SimpleTransformer<BoxCriteria> st(shapes);
                transformed = st.transform();
            }

            if (mergeP == "true")
                merge.merge(transformed);
        }
    }
    else if (func.cat == FUNC_SOLVER) {
        if (func.name == "ScanlineSolver") {
            Scanline solver(shapes);
            solver.solve();
        }
        else if (func.name == "TheSkyIsTheLimitSolver") {
            TheSkyIsTheLimit solver(shapes);
            solver.solve();
        }

        merge.reset();
    }
    else
        throw runtime_error("Unknown type function");
}


/**
 * Definition of the actual grammar
 * using Boost.Spirit.Qi syntax
 * (See CloseEnough.hpp for a human-readable grammar)
 * and error handler
 */
CE_Parser::CE_Parser(vector<Shape>& s) : CE_Parser::base_type(start, "program start") {
    //GRAMMAR BEGIN
    string_			   %= lexeme[+(char_ - '"' - ',' - '(' - ')' - '=')];
    value				= double_[_val = bind(makeNumberValue, _1)]
                          | string_[_val = bind(makeStringValue, _1)];
    parameter			= (string_ >> '=' > value)[_val = bind(makeParameter, _1, _2)];
    parameter_list		= parameter [push_back(phoenix::ref(_val), _1)] % ',' | eps;
    transformer		   %= qi::string("SimpleTransformer")
                          | qi::string("Reset");
    solver			   %= qi::string("ScanlineSolver")
                          | qi::string("TheSkyIsTheLimitSolver");
    function_			= transformer[_val = bind(makeTransFunction, _1)]
                          | solver[_val = bind(makeSolverFunction, _1)];
    instruction			= (function_ > '(' > parameter_list > ')')
                          [_val = bind(makeCall, _1, _2)] > ';';
    instruction_list	= *(instruction [push_back(phoenix::ref(_val), _1)]);
    block				= instruction[push_back(phoenix::ref(_val), _1)]
                          | (qi::string("BEGIN") > instruction_list > qi::string("END"))[_val = _2];
    big_block			= block[phoenix::bind(callEverything, _1, 1, &s)]
                          | (qi::string("DO") > int_ > qi::string("TIMES") > block)
                          [phoenix::bind(callEverything, _4, _2, &s)];
    start				= *(big_block) > qi::eoi;
    //GRAMMAR END
    //Readable names for error output
    string_.name("string");
    value.name("parameter value");
    parameter.name("parameter");
    parameter_list.name("parameter list");
    transformer.name("transformer");
    solver.name("solver");
    function_.name("function");
    instruction.name("instruction");
    instruction_list.name("instruction list");
    block.name("instruction block");
    big_block.name("primary block");
    //Error handler
    on_error<fail>(
        start,
        cerr
        << val("========================================\nCloseEnough parsing error !\nExpecting ")
        << _4
        << val(" here :\n")
        << construct<string>(_3, _2)
        << val("\n========================================")
        << endl
    );
}
