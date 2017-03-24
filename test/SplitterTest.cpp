#include <iostream>

#include "Outer.hpp"
#include "Splitter.hpp"

using namespace std;

int main() {
    int width = 2000, height = 2000, nbSplit = 50;
    Splitter splitter(width, height);

    for (int i = 0 ; i < nbSplit ; i++)
        splitter.split(Point(rand() % width, rand() % height), Point(rand() % width, rand() % height));

    Layout l(splitter.getShapes());
    cout << debugOutputSVG(l);
    }
