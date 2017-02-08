#include <string>
#include <stdexcept>

#include "commonTest.hpp"
#include "common.hpp"
#include "Interpolator.hpp"

using namespace std;

#define R_EPSILON 10e-15

int main() {
    try {
		//x^3 - 6x^2 + 11x - 6 : 1, 2, 3
		vector<complex<double>> roots = Interpolator::cubicRotos(1, -6, 11, -6);
		ASSERT(floatEqual(roots[0].real(), 1., R_EPSILON), "Bad root 1-1");
		ASSERT(floatEqual(roots[1].real(), 2., R_EPSILON), "Bad root 1-2");
		ASSERT(floatEqual(roots[2].real(), 3., R_EPSILON), "Bad root 1-3");
		ASSERT(floatZero(roots[0].imag(), R_EPSILON), "Bad root 1-4");
		ASSERT(floatZero(roots[1].imag(), R_EPSILON), "Bad root 1-5");
		ASSERT(floatZero(roots[2].imag(), R_EPSILON), "Bad root 1-6");

		//x^3 + x^2 + x + 1 : -1, -i, i
		roots = Interpolator::cubicRotos(1, 1, 1, 1);
		ASSERT(floatEqual(roots[0].real(), -1., R_EPSILON), "Bad root 2-1");
		ASSERT(floatZero(roots[1].real(), R_EPSILON), "Bad root 2-2");
		ASSERT(floatZero(roots[2].real(), R_EPSILON), "Bad root 2-3");
		ASSERT(floatZero(roots[0].imag(), R_EPSILON), "Bad root 2-4");
		ASSERT(floatEqual(roots[1].imag(), -1., R_EPSILON), "Bad root 2-5");
		ASSERT(floatEqual(roots[2].imag(), 1., R_EPSILON), "Bad root 2-6");


		//2x^3 - 24x^2 + 108x - 216 = 0 : 6, 3 + 3i, 3 - 3i
		roots = Interpolator::cubicRotos(2, -24, 108, -216);
		ASSERT(floatEqual(roots[0].real(), 3.,  R_EPSILON), "Bad root 3-1");
		ASSERT(floatEqual(roots[1].real(), 3., R_EPSILON), "Bad root 3-2");
		ASSERT(floatEqual(roots[2].real(), 6., R_EPSILON), "Bad root 3-3");
		ASSERT(floatEqual(roots[0].imag(), 3., R_EPSILON), "Bad root 3-4");
		ASSERT(floatEqual(roots[1].imag(), -3., R_EPSILON), "Bad root 3-5");
		ASSERT(floatZero(roots[2].imag(), R_EPSILON), "Bad root 3-6");

		//x^2 - 10x + 1 = 0 : 5 +/- sqrt(6)
		roots = Interpolator::cubicRotos(0, 1, -10, 1);
		ASSERT(floatEqual(roots[0].real(), 5. + 2. * sqrt(6.), R_EPSILON), "Bad root 4-1");
		ASSERT(floatEqual(roots[1].real(), 5. - 2. * sqrt(6.), R_EPSILON), "Bad root 4-2");
		ASSERT(floatZero(roots[0].imag(), R_EPSILON), "Bad root 4-3");
		ASSERT(floatZero(roots[1].imag(), R_EPSILON), "Bad root 4-4");

		//42x - 42 : 1
		roots = Interpolator::cubicRotos(0, 0, 42, -42);
		ASSERT(floatEqual(roots[0].real(), 1., R_EPSILON), "Bad root 5-1");
        }

    catch (exception& e) {
        cerr << "Failed : " << e.what() << endl;
		return EXIT_FAILURE;
        }

    return EXIT_SUCCESS;
    }
