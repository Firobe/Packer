#ifndef ROTOS__HPP
#define ROTOS__HPP

#include <vector>
#include <complex>

#include "common.hpp"

namespace Interpolator {
std::vector<std::complex<double>> cubicRotos(double a, double b, double c, double d);
bool intersects(double ax, double ay, double bx, double by, double cx, double cy,
                double dx, double dy, double, double&, double&);
double norm(double ax, double ay, double bx, double by, double cx, double cy, double dx,
            double dy,
            double& Ex, double& Ey);

std::vector<Point> subdivision(Point& p1, Point& p2, Point& p3, Point& p4);
Point middlePoint(Point& p1, Point& p2);
}
#endif
