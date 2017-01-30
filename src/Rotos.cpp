#include <complex>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>

#include "Rotos.hpp"
#include "Parser.hpp"
#include "Log.hpp"

#define MOCHE_EPSILON 10e-10 //Precision on the norm dichotomy (and c/d)
#define DEGREE_EPSILON 10e-2 //Precision to approximate cubic or quadratic polynomial to quadratic or linear
using namespace std;

/**
 * Input : polynomial at^3 + bt^2 + ct + d
 * Returns a vector of (potentially complex) roots
 * If a root is multiple, it is not duplicated
 */
vector<complex<double>> rotos::cubicRotos(double a, double b, double c, double d) {
    //It's Maths all over again
    vector<complex<double>> roots;

    if (abs(a) < DEGREE_EPSILON * max(abs(b),
                                      abs(c))) { //If a is negligible before b and c, a = 0
        if (abs(b) < DEGREE_EPSILON * abs(c)) { //Idem for b before c
            if (abs(c) < MOCHE_EPSILON) { //If c is really small, approximate to constant (c = 0)
                if (abs(d) < MOCHE_EPSILON) //If our polynomial is 0 = 0, return an arbitrary real root
                    return {complex<double>(0.5, 0)};
                else {//If our polynomial is x = 0 (x != 0), undefined behaviour (returning complex root)
                    LOG(error) << "POLYNOME ZOUGLOU D = " << d << endl;
                    return {complex<double>(0, 0.5)};
                }
            }

            //Root of a linear polynomial
            roots.push_back(- d / c);
            return roots;
        }

        //Roots of a quadratic polynomial
        double delta = c * c - 4 * b * d;
        roots.push_back((-c + sqrt(complex<double>(delta))) / (2 * b));
        roots.push_back((-c - sqrt(complex<double>(delta))) / (2 * b));
        return roots;
    }

    //General case : cubic polynomial
    //Notations used : https://en.wikipedia.org/wiki/Cubic_function#Algebraic_solution
    complex<double> zeta = -1 / 2. + 1 / 2. * sqrt(3) * complex<double>(0., 1.);
    double delta = 18 * a * b * c * d - 4 * b * b * b * d + b * b * c * c - 4 * a * c * c * c
                   - 27 * a * a * d * d;
    double delta0 = b * b - 3 * a * c;
    double delta1 = 2 * b * b * b - 9 * a * b * c + 27 * a * a * d;
    double signe = delta1 > 0 ? 1 : -1;
    complex<double> C = pow(complex<double>((delta1 + signe * sqrt(complex<double>
                                            (delta1 * delta1 -
                                                    4 * delta0 * delta0 * delta0))) / 2.), 1 / 3.);

    if (delta == 0) {
        if (delta0 == 0) {
            //Triple real root
            roots.push_back(-b / (3 * a));
        }
        else {
            //Double real root
            roots.push_back((9 * a * d - b * c) / (2 * delta0));
            //And simple real root
            roots.push_back((4 * a * b * c - 9 * a * a * d - b * b * b) / (a * delta0));
        }
    }
    else
        for (int k : {
                    0, 1, 2
                }) {
            //Three distinct roots
            complex<double> p = pow(zeta, k);
            roots.push_back(-1. / (3 * a) * (b + p * C + delta0 / (p * C)));
        }
    return roots;
}

/**
 * Input : bezier curve defined by its two anchor points A = (ax, ay) and D = (dx, dy) and its two
 * control points (bx, by) and (cx, cy).
 *
 * Returns true if one of the lines (on both sides of [AD])
 * parallel to [AD] and distant of <distance> intersects the curve.
 *
 * If it does, sets (whereX, whereY) to the first point of intersection detected;
 */
bool rotos::intersects(double ax, double ay, double bx, double by, double cx, double cy,
                       double dx, double dy,
                       double distance, double& whereX, double& whereY) {
    double backAx = ax, backAy = ay; //Save the position of A
    double frac, a, b, c, d, snorm, tx, ty, ux, uy, vx, vy;
    //Take back A to the origin and translates B, C, D appropriately
    bx -= ax;
    cx -= ax;
    dx -= ax;
    ax = 0.;
    by -= ay;
    cy -= ay;
    dy -= ay;
    ay = 0.;

    //For both sides of [AD]
    for (int signe : {
                1, -1
            }) {
        //Norm of the vector (AD)
        snorm = sqrt(dx * dx + dy * dy);
        tx = - dy * signe * distance / snorm;
        ty = dx * signe * distance / snorm;
        ux = tx;
        uy = ty;
        vx = tx + dx;
        vy = ty + dy;
        //The line formed by (ux, uy), (vx, vy) is parallel to [AD] and distant of <distance> (on the right side)

        //(AD) is closer (angularly speaking) from the Y axis than from the X axis, swap the coordinates
        //So that frac (defined by (AD)'s slope) is finite
        if (abs(vy - uy) > abs(vx - ux)) {
            //And now, more maths
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

        //The polynomial at^3 + bt^2 + ct + d is now defined so that its roots are the points of intersection
        //between (UV) and the bezier curve
        //Compute the said roots (may be complex)
        vector<complex<double>> roots = cubicRotos(a, b, c, d);

        for (auto r : roots) {
            //If the root is real (meaning there IS an intersection)
            if (abs(r.imag()) < MOCHE_EPSILON) {
                //Check that the intersection occurs on the curve's domain ([0, 1])
                if (r.real() < 1. && r.real() > 0.) {
                    //Evalutes the curve where the intersection occurs and returns that point
                    double t = r.real();
                    whereX = backAx * (1 - t) * (1 - t) * (1 - t) + 3 * (bx + backAx) * t * (1 - t) * (1 - t)
                             + 3 * (cx + backAx) * t * t * (1 - t) + (dx + backAx) * t * t * t;
                    whereY = backAy * (1 - t) * (1 - t) * (1 - t) + 3 * (by + backAy) * t * (1 - t) * (1 - t)
                             + 3 * (cy + backAy) * t * t * (1 - t) + (dy + backAy) * t * t * t;
                    return true;
                }
            }
        }
    }
    //There were no intersections
    return false;
}

/**
 * Sets (Ex, Ey) to the point which maximizes the distance between the Bezier curve
 * (defined by its two anchor points A = (ax, ay) and D = (dx, dy) and its two control points (bx, by)
 * and (cx, cy)) and (AD) and returns the said distance.
 */
double rotos::norm(double ax, double ay, double bx, double by, double cx, double cy,
                   double dx, double dy,
                   double& Ex, double& Ey) {
    //The result is computed through a dichotomy
    double upper = 128;
    double lower = 0;
    //Length of [AD]
    double longDist = sqrt((ax - dx) * (ax - dx) + (ay - dy) * (ay - dy));
    //Length of [AB] + [BC] + [CD]
    double dist = sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by)) +
                  sqrt((bx - cx) * (bx - cx) + (by - cy) * (by - cy)) +
                  sqrt((cx - dx) * (cx - dx) + (cy - dy) * (cy - dy));

    //Using these length, check that A, B, C and D are not colinear
    if (abs(longDist - dist) < MOCHE_EPSILON) {
        return 0.;
    }

    //Initialization of the dichotomy (find a distance where the line does not intersect)
    while (intersects(ax, ay, bx, by, cx, cy, dx, dy, upper, Ex, Ey)) {
        lower = upper;
        upper *= 2;
    }

    //Dichotomy (Ensure that we have a higher precision than the interpolation)
    while (upper - lower > BEZIER_TOLERANCE * 0.001) {
        double mid = (upper + lower) / 2;

        //If our line intersects, the extremum is higher
        if (intersects(ax, ay, bx, by, cx, cy, dx, dy, mid, Ex, Ey)) {
            lower = mid;
        }
        //If not, it is lower
        else {
            upper = mid;
        }
    }

    return (upper + lower) / 2;
}
