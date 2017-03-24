#include <ctime>
#include <typeinfo>
#include <chrono>
#include <boost/type_index.hpp>

#include "solver/ScanlineSolver.hpp"
#include "solver/LineSolver.hpp"
#include "solver/MultilineSolver.hpp"
#include "solver/FreezeSolver.hpp"
#include "solver/ProbaSolver.hpp"
#include "Splitter.hpp"
#include "Parser.hpp"
#include "Outer.hpp"

#define RANGE 10000

using Clock = std::chrono::system_clock;
using namespace std;

template <typename T>
void testSolver(Layout shapes) {
	Parser::setDims(Point(RANGE * 3, RANGE * 3));
    cerr << "Testing : " << boost::typeindex::type_id<T>().pretty_name() << endl;
    T s(shapes, {});
    auto start = Clock::now();
    s.solve();
    auto end = Clock::now();
    int elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();
    cerr << elapsed << " microseconds elapsed" << endl;
    cerr << "COMPRESSION RATIO : " << compressionRatio(shapes) << endl << endl;;
    }

int main() {
    srand(time(0));

    int splitNb = rand() % 42 + 21;
    Splitter cut1(RANGE, RANGE);
    Splitter cut2(RANGE, RANGE);

    for (int i = 0 ; i < splitNb ; ++i) {
        cut1.split(Point(rand() % RANGE, rand() % RANGE), Point(rand() % RANGE, rand() % RANGE));
        int r = rand() % RANGE;
        bool vr = rand() % 2;
        cut2.split(Point(r, r), vr ? Point(r, RANGE) : Point(RANGE, r));
        }

    Layout rects(cut1.getShapes()); //Rectangles
	Layout triangles(cut2.getShapes()); //Triangles

    for (auto && s : {
                rects, triangles
            }) {
		cerr << "-------------------------------------" << endl;
        testSolver<Solver>(s);
        testSolver<LineSolver>(s);
        testSolver<MultilineSolver>(s);
        testSolver<ScanlineSolver>(s);
        //testSolver<FreezeSolver>(s);
        testSolver<ProbaSolver>(s);
        }
    return EXIT_SUCCESS;
    }
