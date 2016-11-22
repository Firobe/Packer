#ifndef SOLVER__H
#define SOLVER__H

#include <rapidxml_ns/rapidxml_ns.hpp>
#include <string>
#include <algorithm>
#include <sstream>


#include "Shape.hpp"


/**
 * Solver class. An abstract class
 * as a base for solver algorithms.
 *
 * An inherited algorithm only has to
 * implement the solve() method, which
 * should modify _shapes and nothing
 * else.
 */
class Solver {
protected:
  std::vector<Shape>& _shapes;
  int _binNumber;
  double _width, _height;
public:
  Solver(std::vector<Shape>& s, double w, double h) : _shapes(s), _binNumber(0), _width(w), _height(h) {}
  void solve() = delete;
  std::string debugOutputSVG();
};

#endif
