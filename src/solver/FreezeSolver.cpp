#include <iostream>
#include <vector>
#include <numeric>

#include "FreezeSolver.hpp"
#include "Log.hpp"
#include "common.hpp"
#include "Display.hpp"

#define PHYSICS_GRAVITY_TIME        100
#define PHYSICS_STABILIZATION_TIME  1000
#define PHYSICS_ALL_TIME			(PHYSICS_GRAVITY_TIME + PHYSICS_STABILIZATION_TIME)
#define GRAVITY_STEP                10.
#define BOUNCE_STEP                 1.


using namespace std;


void FreezeSolver::preSolve() {
    for (unsigned i = 0; i < _shapes.size(); ++i)
        rotateToBestAngle(_shapes[i]);
}


// Principe: On balance une pièce au dessus
void FreezeSolver::solveBin() {
    Box box;
    std::list<unsigned> indiceCopy;
    indiceCopy.resize(_shapes.size());
    iota(indiceCopy.begin(), indiceCopy.end(), 0);

    for (auto && i : _indices)
        awayStartingPoint(_shapes[i]);

    for (list<unsigned>::iterator i = _indices.begin(); i != _indices.end(); ++i) {
        randomStartingPoint(_shapes[*i]);
        wiggle(_shapes[*i]);

        for (unsigned t = 0 ; t < PHYSICS_ALL_TIME ; ++t) {
            _shapes[*i].envelope(box);

            if (box.max_corner().y() > (_binNumber + 1) * Parser::getDims().y()) {
                bounceUp(_shapes[*i]); // Move up + random slight rotation
            }
            else if (box.min_corner().x() < 0)
                bounceRight(_shapes[*i]);
            else if (box.max_corner().x() > Parser::getDims().x())
                bounceLeft(_shapes[*i]);
            else {
                for (auto && j : indiceCopy) {
                    if (j < *i && _shapes[*i].intersectsWith(_shapes[j])) {
                        if (rand() % 3 == 0)
                            bounceUp(_shapes[*i]);
                        else if (rand() % 2 == 0)
                            bounceLeft(_shapes[*i]);
                        else
                            bounceRight(_shapes[*i]);
                    }
                }

                if (t < PHYSICS_GRAVITY_TIME)
                    gravityMoveDown(_shapes[*i]);
            }
        }

        // Checking if stabilization worked, else shape is sent to next bin
        bool stabilized = true;
        _shapes[*i].envelope(box);

        if (box.min_corner().x() < 0 || box.max_corner().x() > Parser::getDims().x() ||
                box.min_corner().y() < _binNumber * Parser::getDims().y() ||
                box.max_corner().y() > (_binNumber + 1) * Parser::getDims().y())
            stabilized = false;

        if (stabilized != false) {
            for (auto && j : indiceCopy) {
                if (j < *i && _shapes[*i].intersectsWith(_shapes[j])) {
                    stabilized = false;
                    break;
                }
            }
        }

        if (stabilized == true)
            markPacked(i);
    }
}


void FreezeSolver::awayStartingPoint(Shape& shape) {
    Box box;
    shape.envelope(box);
    // Translation above
    shape.translate(-box.min_corner().x() - Parser::getDims().x() * SPACE_COEF,
                    -box.min_corner().y());
}

void FreezeSolver::randomStartingPoint(Shape& shape) {
    Box box;
    shape.envelope(box);
    // Translation to bin origin
    shape.translate(-box.min_corner().x() + Parser::getDims().x() / 2,
                    -box.min_corner().y() + Parser::getDims().y() * _binNumber * SPACE_COEF);
}




void FreezeSolver::gravityMoveDown(Shape& shape) {
    shape.translate(0., GRAVITY_STEP);
}

void FreezeSolver::wiggle(Shape& shape) {
    float randomX, randomY, randomAngle;
    randomX = rand() % 101 - 50.;
    // First bin randomY can be: -1,0,1
    // Then the shape starts flying upwards
    randomY = rand() % 3 - 1. - (_binNumber / 5.);
    randomAngle = rand() % 3 - 1.;
    shape.translate(randomX, randomY);
    shape.rotate(randomAngle);
}


void FreezeSolver::bounceUp(Shape& shape) {
    shape.translate(0., -BOUNCE_STEP);
    wiggle(shape);
}

void FreezeSolver::bounceLeft(Shape& shape) {
    shape.translate(-BOUNCE_STEP, 0.);
    wiggle(shape);
}

void FreezeSolver::bounceRight(Shape& shape) {
    shape.translate(BOUNCE_STEP, 0.);
    wiggle(shape);
}


