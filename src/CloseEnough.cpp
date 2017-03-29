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
#include "transformer/SimpleTransformer.hpp"
#include "transformer/HoleTransformer.hpp"
#include "solver/ScanlineSolver.hpp"
#include "solver/FreezeSolver.hpp"
#include "solver/MultilineSolver.hpp"
#include "solver/LineSolver.hpp"
#include "solver/ProbaSolver.hpp"

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

//Define registries : each type will be added as its name in the grammar
//and will be instanciated when parsed
using TransformerRegistry = Registry<Transformer,
      boost::mpl::set<
      SimpleTransformer,
      HoleTransformer
      >::type>;

using SolverRegistry = Registry<Solver,
      boost::mpl::set<
      ScanlineSolver,
      MultilineSolver,
      LineSolver,
      FreezeSolver,
      ProbaSolver
      >::type>;

/**
 * Given a list of parameters and a key string,
 * returns if such a key is found and fills value with its value
 * if found
 */
template<>
bool getParameter(std::vector<Parameter> p, std::string key, Value& value) {
    auto it = std::find_if(p.begin(), p.end(), [key](const Parameter & p) {
        return p.name == key;
    });

    if (it == p.end())
        return false;

    value = it->value;
    return true;
}

/**
 * Stream operator for Value
 */
ostream& operator<< (ostream& os, const Value& c) {
    os << c;
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
void callEverything(vector<Call> block, int n, Layout* shapes) {
    for (int i = 0 ; i < n ; ++i)
        for (auto && j : block)
            j(*shapes);
}

/**
 * Given a Call struct, make the appropriate
 * function calls with appropriate parameters
 */
void Call::operator()(Layout& shapes) {
    static Merger merge(shapes);

    if (func.cat == FUNC_TRANSFORMER) {
        vector<vector<unsigned>> transformed;
        string mergeP;

        if (!getParameter(params, "merge", mergeP))
            mergeP = "true";

        Transformer* t = TransformerRegistry::instanciate(func.name, shapes, params);
        transformed = t->transform();

        if (mergeP == "true")
            merge.merge(transformed);

        delete t;
    }
    else if (func.cat == FUNC_SOLVER) {
        Solver* s = SolverRegistry::instanciate(func.name, shapes, params);
        s->solve();
        merge.reset();
        delete s;
    }
    else
        throw runtime_error("Unknown type function");
}

/**
 * Generates a grammar rule based
 * on a registry.
 */
template<typename Reg>
void makeRule(qi::rule<std::string::iterator, std::string(), ascii::space_type>& r) {
    auto m = Reg::_fact;

    for (auto && p : m)
        r %= r.copy() | qi::string(string(p.first));
}

/**
 * Definition of the actual grammar
 * using Boost.Spirit.Qi syntax
 * (See CloseEnough.hpp for a human-readable grammar)
 * and error handler
 */
CE_Parser::CE_Parser(Layout& s) : CE_Parser::base_type(start, "program start") {
    static qi::real_parser<double, qi::strict_real_policies<double>> const strict_double;
    TransformerRegistry::init();
    SolverRegistry::init();
    //GRAMMAR BEGIN
    string_			   %= lexeme[+(char_ - '"' - ',' - '(' - ')' - '=')];
    value			   %= strict_double
                          | int_
                          | string_;
    parameter			= (string_ >> '=' > value)[_val = bind(makeParameter, _1, _2)];
    parameter_list		= parameter [push_back(phoenix::ref(_val), _1)] % ',' | eps;
    makeRule<TransformerRegistry>(transformer);
    makeRule<SolverRegistry>(solver);
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
