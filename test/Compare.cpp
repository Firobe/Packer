#include <ctime>
#include <typeinfo>
#include <chrono>
#include <boost/type_index.hpp>

#include "ScanlineSolver.hpp"
#include "ToInfinityAndBeyondSolver.hpp"
#include "TheSkyIsTheLimitSolver.hpp"
#include "Splitter.hpp"

#define RANGE 10000

using Clock = std::chrono::system_clock;
using namespace std;

template <typename T>
void testSolver(vector<Shape> shapes) {
    cerr << "Testing : " << boost::typeindex::type_id<T>().pretty_name() << endl;
    T s(shapes, Point(RANGE * 3, RANGE * 3));
    auto start = Clock::now();
    s.solve();
    auto end = Clock::now();
    int elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();
    cerr << elapsed << " microseconds elapsed" << endl;
    cerr << "COMPRESSION RATIO : " << s.compressionRatio() << endl << endl;;
    }

int main() {
    //srand(time(0));

    int splitNb = rand() % 42 + 21;
    Splitter splot(RANGE, RANGE);
    Splitter splout(RANGE, RANGE);

    for (int i = 0 ; i < splitNb ; ++i) {
        splot.split(Point(rand() % RANGE, rand() % RANGE), Point(rand() % RANGE, rand() % RANGE));
        int r = rand() % RANGE;
        bool vr = rand() % 2;
        splout.split(Point(r, r), vr ? Point(r, RANGE) : Point(RANGE, r));
        }

    vector<Shape> shapes = splot.getShapes();
    vector<Shape> shapes2 = splout.getShapes();

    for (auto && s : {
                shapes, shapes2
            }) {
		cerr << "-------------------------------------" << endl;
        testSolver<ScanlineSolver>(s);
        testSolver<ToInfinityAndBeyondSolver>(s);
        testSolver<TheSkyIsTheLimitSolver>(s);
        testSolver<Solver>(s);
        }
    return EXIT_SUCCESS;
    }
