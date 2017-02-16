#ifndef CLOSE_ENOUGH__HPP
#define CLOSE_ENOUGH__HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <iostream>
#include <string>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

using qi::double_;
using qi::int_;
using qi::_1;
using qi::_2;
using qi::_3;
using qi::_4;
using qi::eps;
using qi::_val;
using qi::lit;
using qi::lexeme;
using ascii::space;
using ascii::char_;
using phoenix::ref;
using phoenix::bind;

/*
Grammaire
============

program = big_block | big_block program
big_block = bloc | 'DO' NUM 'TIMES' bloc
bloc = instruction | 'BEGIN' instruction_list 'END'
instruction_list = instruction | instruction instruction_list
instruction = fonction '(' arguments ')' ';'
fonction = transformer | solver
transformer = 'SimpleTransformer'
solver = 'ScanlineSolver' | 'TheSkyIsTheLimitSolver'
arguments = argument | argument ',' arguments
argument = STRING '=' primitive
primitive = NUM | STRING

Features
=========
Instructions séparées par ';'
Blocs d'instructions délimités par BEGIN et END
On peut exécuter X fois un bloc d'instruction avec DO X TIMES ...
Une instruction consiste en : fonction(param=valeur, param2=val2, ...)

Exemple
=========
DO 2 TIMES
BEGIN
SimpleTransformer(criteria=IntersectionCriteria, rotateStep=30, translateStep=2);
SimpleTransformer(criteria=BoxCriteria, rotateStep=30, translateStep=2);
END
ScanlineSolver();
*/


struct Value {
    bool isNum;
    double number;
    std::string str;
};

std::ostream& operator<< (std::ostream& os, const Value& c) {
    os << (c.isNum ? std::to_string(c.number) : c.str);
    return os;
}

struct Parameter {
    std::string name;
    Value value;
};

bool getParameter(std::vector<Parameter> p, std::string key, std::string& value) {
    auto it = std::find_if(p.begin(), p.end(), [key](const Parameter & p) {
        return p.name == key;
    });

    if (it == p.end() || it->value.isNum)
        return false;

    value = it->value.str;
    return true;
}

bool getParameter(std::vector<Parameter> p, std::string key, double& value) {
    auto it = std::find_if(p.begin(), p.end(), [key](const Parameter & p) {
        return p.name == key;
    });

    if (it == p.end() || !it->value.isNum)
        return false;

    value = it->value.number;
    return true;
}

enum funcCategory { FUNC_TRANSFORMER, FUNC_SOLVER };
struct Function {
    std::string name;
    funcCategory cat;
};

struct Call {
    Function func;
    std::vector<Parameter> params;

    void operator()(std::vector<Shape>& shapes) {
        static Merger merge(shapes);

        if (func.cat == FUNC_TRANSFORMER) {
            std::vector<std::vector<unsigned>> transformed;

            if (func.name == "SimpleTransformer") {
                std::string criteria, mergeP;

                if (!getParameter(params, "criteria", criteria))
                    throw std::runtime_error("Please specify a criteria for the transformer");

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
            throw std::runtime_error("Unknown type function");
    }
};

std::ostream& operator<< (std::ostream& os, const Call& call) {
    os << "Calling " << call.func.name << " (" << call.func.cat << ")" << std::endl;
    os << "with parameters ";

    for (auto && i : call.params)
        os << "(" << i.name << ", " << i.value << ") ";

    os << std::endl;
    return os;
}

Value makeNumberValue(double d) {
    return {true, d, ""};
}
Value makeStringValue(std::string s) {
    return {false, 0., s};
}
Parameter makeParameter(std::string s, Value v) {
    return {s, v};
}
Function makeTransFunction(std::string s) {
    return {s, FUNC_TRANSFORMER};
}
Function makeSolverFunction(std::string s) {
    return {s, FUNC_SOLVER};
}
Call makeCall(Function f, std::vector<Parameter> p) {
    return {f, p};
}

void callEverything(std::vector<Call> block, int n, std::vector<Shape>* shapes) {
    for (int i = 0 ; i < n ; ++i)
        for (auto && j : block)
            j(*shapes);
}

template <typename Iterator>
struct CE_Parser : qi::grammar<Iterator, void(), ascii::space_type> {

    qi::rule<Iterator, Value, ascii::space_type> 					value;
    qi::rule<Iterator, std::string(), ascii::space_type> 			string_;
    qi::rule<Iterator, Parameter(), ascii::space_type> 				parameter;
    qi::rule<Iterator, std::vector<Parameter>(), ascii::space_type> parameter_list;
    qi::rule<Iterator, std::string(), ascii::space_type> 			transformer;
    qi::rule<Iterator, std::string(), ascii::space_type> 			solver;
    qi::rule<Iterator, Function(), ascii::space_type> 				function_;
    qi::rule<Iterator, Call(), ascii::space_type> 					instruction;
    qi::rule<Iterator, std::vector<Call>(), ascii::space_type> 		instruction_list;
    qi::rule<Iterator, std::vector<Call>(), ascii::space_type> 		block;
    qi::rule<Iterator, void(), ascii::space_type> 					big_block;
    qi::rule<Iterator, void(), ascii::space_type> 					start;

    CE_Parser(std::vector<Shape>& s) : CE_Parser::base_type(start) {
        string_ %= 			lexeme[+(char_ - '"' - ',' - '(' - ')' - '=')];
        value = 			double_[_val = bind(makeNumberValue, _1)]
                            | string_[_val = bind(makeStringValue, _1)];
        parameter =	(string_ >> '=' >> value)[_val = bind(makeParameter, _1, _2)];
        parameter_list =	parameter [phoenix::push_back(ref(_val), _1)] % ',' | eps;
        transformer %= 		qi::string("SimpleTransformer")
                            | qi::string("Reset");
        solver %= 			qi::string("ScanlineSolver")
                            | qi::string("TheSkyIsTheLimitSolver");
        function_ =			transformer[_val = bind(makeTransFunction, _1)]
                            | solver[_val = bind(makeSolverFunction, _1)];
        instruction =	(function_ >> '(' >> parameter_list >> ')')[_val = bind(makeCall, _1, _2)];
        instruction_list =	instruction [phoenix::push_back(ref(_val), _1)] % ';';
        block =				instruction[phoenix::push_back(ref(_val), _1)]
                            | (qi::string("BEGIN") >> instruction_list >> qi::string("END"))[_val = _2];
        big_block = 		block[phoenix::bind(callEverything, _1, 1, &s)]
                            | (qi::string("DO") >> int_ >> qi::string("TIMES") >> block)
                            [phoenix::bind(callEverything, _4, _2, &s)];
        start = 			*(big_block);
    }
};

#endif
