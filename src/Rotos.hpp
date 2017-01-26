#ifndef ROTOS__HPP
#define ROTOS__HPP

#include <vector>
#include <complex>

namespace rotos {
	std::vector<std::complex<double>> cubicRotos(double a, double b, double c, double d);
	bool intersects(double ax, double ay, double bx, double by, double cx, double cy, double dx, double dy, double, double&, double&);
	double norm(double ax, double ay, double bx, double by, double cx, double cy, double dx, double dy,
			double& Ex, double& Ey);

}
#endif
