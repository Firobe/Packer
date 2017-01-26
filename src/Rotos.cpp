#include <complex>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>

#include "Rotos.hpp"
#include "Log.hpp"

#define MOCHE_EPSILON 10e-12
using namespace std;

vector<complex<double>> rotos::cubicRotos(double a, double b, double c, double d) {
    //It's Maths all over again
    vector<complex<double>> roots;

    if (a == 0.) {
        if (b == 0.) {
            if (c == 0.){
				if(d == 0.)
					return {complex<double>(0.5, 0)};
				else{
					LOG(fatal) << "POLYNOME ZOUGLOU D = " << d << endl;
                	throw runtime_error("You wat : polynôme de degré 3 constant et pas plat");
				}
			}

            roots.push_back(- d / c);
            return roots;
            }

        double delta = c * c - 4 * b * d;
        roots.push_back((-c + sqrt(complex<double>(delta))) / (2 * b));
        roots.push_back((-c - sqrt(complex<double>(delta))) / (2 * b));
        return roots;
        }

    complex<double> zeta = -1 / 2. + 1 / 2. * sqrt(3) * complex<double>(0., 1.);
    double delta = 18 * a * b * c * d - 4 * b * b * b * d + b * b * c * c - 4 * a * c * c * c
                   - 27 * a * a * d * d;
    double delta0 = b * b - 3 * a * c;
    double delta1 = 2 * b * b * b - 9 * a * b * c + 27 * a * a * d;
    double signe = delta1 > 0 ? 1 : -1;
    complex<double> C = pow(complex<double>((delta1 + signe * sqrt(complex<double>(delta1 * delta1 -
                                            4 * delta0 * delta0 * delta0))) / 2.), 1 / 3.);

    if (delta == 0) {
        if (delta0 == 0) {
            roots.push_back(-b / (3 * a));
            roots.push_back(-b / (3 * a));
            roots.push_back(-b / (3 * a)); //Racine triple
            }
        else {
            roots.push_back((9 * a * d - b * c) / (2 * delta0));
            roots.push_back((9 * a * d - b * c) / (2 * delta0)); //Racine double
            roots.push_back((4 * a * b * c - 9 * a * a * d - b * b * b) / (a * delta0));
            }
        }
    else
        for (int k : {
                    0, 1, 2
                }) {
            complex<double> p = pow(zeta, k);
            roots.push_back(-1. / (3 * a) * (b + p * C + delta0 / (p * C)));
            }
    return roots;
    }

bool rotos::intersects(double ax, double ay, double bx, double by, double cx, double cy, double dx, double dy,
                double distance) {
    for (int signe : {
                -1, 1
            }) {
        //On ramène à l'origine
        bx -= ax;
        cx -= ax;
        dx -= ax;
        ax = 0.;
        by -= ay;
        cy -= ay;
        dy -= ay;
        ay = 0.;

        LOG(trace) << "INTERSECTION TEST" << endl;
        double snorm = sqrt((ax - dx) * (ax - dx) + (ay - dy) * (ay - dy));
        double tx = (ay - dy) * signe * distance / snorm;
        double ty = (dx - ax) * signe * distance / snorm;
        double ux = tx + ax;
        double uy = ty + ay;
        double vx = tx + dx;
        double vy = ty + dy;
        double frac, a, b, c, d;

        if (vy - uy > vx - ux) { //Ca monte vite
            frac = (vx - ux) / (vy - uy);
            a = (-3 * by + 3 * cy - dy) * frac + 3 * bx - 3 * cx + dx;
            b = frac * (6 * by - 3 * cy) - 6 * bx + 3 * cx;
            c = 3 * bx - 3 * frac * by;
            d = frac * uy - ux;
            }
        else {
            frac = (vy - uy) / (vx - ux);
            a = (-3 * bx + 3 * cx - dx) * frac + 3 * by - 3 * cy + dy;
            b = frac * (6 * bx - 3 * cx) - 6 * by + 3 * cy;
            c = 3 * by - 3 * frac * bx;
            d = frac * ux - uy;
            }

        LOG(trace) << "ux " << ux << "\nuy " << uy << "\nvx " << vx << "\nvy " << vy << endl;
        LOG(trace) << "frac " << frac << endl;
        LOG(trace) << "a " << a << "\nb " << b << "\nc " << c << "\nd " << d << endl;
		vector<complex<double>> roots ;
		try{
			roots = cubicRotos(a, b, c, d);
		}
		catch(...){
			LOG(fatal) << "A : " << ax << " ; " << ay << endl << "D : " << dx << " ; " << dy << endl;
			throw runtime_error("STAHP");
		}

        LOG(trace) << "Roots : ";

        for (auto r : roots) {
            LOG(trace) << r << ", ";

            if (abs(r.imag()) < MOCHE_EPSILON) { //Réelle
                if (r.real() < 1. && r.real() > 0.) {
                    LOG(trace) << "Intersects for signe = " << signe << endl;
                    return true;
                    }
                }
            }

        LOG(trace) << endl;
        }
    LOG(trace) << "No intersection" << endl;
    return false;
    }

double rotos::norm(double ax, double ay, double bx, double by, double cx, double cy, double dx, double dy) {
    double upper = 42;
    double lower = 0;
	if(abs(ax - dx) < 10e-14 && abs(ay - dy) < 10e-14)
		return 0.;

    while (intersects(ax, ay, bx, by, cx, cy, dx, dy, upper)) upper *= 2;

    LOG(trace) << "Upper begin : " << upper << endl;

    while (upper - lower > 10e-2) {
        double mid = (upper + lower) / 2;
        LOG(trace) << "Lower, upper : " << lower << " ; " << upper << endl;

        if (intersects(ax, ay, bx, by, cx, cy, dx, dy, mid))
            lower = mid;
        else upper = mid;
        }

    return (upper + lower) / 2;
    }
