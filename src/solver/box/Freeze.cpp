#include <iostream>
#include <boost/geometry/algorithms/envelope.hpp>
#include <boost/geometry/algorithms/intersects.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/algorithms/comparable_distance.hpp>
#include <boost/geometry/algorithms/intersection.hpp>
#include <boost/geometry/algorithms/convex_hull.hpp>
#include <boost/geometry/algorithms/centroid.hpp>
#include <vector>


#include "Freeze.hpp"
#include "Log.hpp"
#include "common.hpp"
#include "Display.hpp"

#define PHYSICS_GRAVITY_TIME        100
#define PHYSICS_STABILIZATION_TIME  1000
#define PHYSICS_ALL_TIME			(PHYSICS_GRAVITY_TIME + PHYSICS_STABILIZATION_TIME)
#define GRAVITY_STEP                10.
#define BOUNCE_STEP                 1.
#define REPEL_STEP_MULTIPLE			5.


using namespace std;


void Freeze::preSolve() {
    for (unsigned i = 0; i < _shapes.size(); ++i)
        rotateToBestAngle(_shapes[i]);
}


// Principe: On balance une pièce au dessus
void Freeze::solveBin(){
    Box box;
    std::list<unsigned> indiceCopy;
    indiceCopy.resize(_shapes.size());
    iota(indiceCopy.begin(), indiceCopy.end(), 0);
    
    for(auto && i : _indices){
		awayStartingPoint(_shapes[i]);
	}
    
    for(list<unsigned>::iterator i = _indices.begin(); i != _indices.end(); ++i) {

        randomStartingPoint(_shapes[*i]);
        wiggle(_shapes[*i]);
        

        for(unsigned t = 0 ; t < PHYSICS_ALL_TIME ; ++t){
            bg::envelope(_shapes[*i].getMultiP(), box);

            if(box.max_corner().y() > (_binNumber+1) * Parser::getDims().y()){
                bounceUp(_shapes[*i]); // Move up + random slight rotation
            } else if(box.min_corner().x() < 0){
                bounceRight(_shapes[*i]);
            } else if(box.max_corner().x() > Parser::getDims().x()){
                bounceLeft(_shapes[*i]);
            } else{
                for(auto && j : indiceCopy){
                    if(j < *i && bg::intersects(_shapes[*i].getMultiP(), _shapes[j].getMultiP())){
                        if(rand()%3 == 0){
							bounceUp(_shapes[*i]);
						} else if(rand()%2 ==0){
							bounceLeft(_shapes[*i]);
						} else{
							bounceRight(_shapes[*i]);
						}
							
                        //bounceShape(_shapes[*i],_shapes[j]);
                    }
                }

                if(t < PHYSICS_GRAVITY_TIME){
					gravityMoveDown(_shapes[*i]);
                }

            }
        }

        // Checking if stabilization worked, else shape is sent to next bin
        bool stabilized = true;
        
        bg::envelope(_shapes[*i].getMultiP(), box);
        if(box.min_corner().x() < 0 || box.max_corner().x() > Parser::getDims().x() ||
           box.min_corner().y() < _binNumber * Parser::getDims().y() ||
           box.max_corner().y() > (_binNumber+1) * Parser::getDims().y()){
			stabilized = false;
		   }
		   
		if(stabilized != false){
			for(auto && j : indiceCopy){
				if(j < *i && bg::intersects(_shapes[*i].getMultiP(), _shapes[j].getMultiP())){
					stabilized = false;
					break;
                }
			}
		}
		
		if(stabilized == true){
			LOG(info) << "Packed : " << *i << endl;
			markPacked(i);
		}
		
    }
}


void Freeze::awayStartingPoint(Shape& shape){
	Box box;
	bg::envelope(shape.getMultiP(), box);
	
	// Translation above
	translate<Shape>(shape, -box.min_corner().x() - Parser::getDims().x() * SPACE_COEF,
							-box.min_corner().y());
	
	
}

void Freeze::randomStartingPoint(Shape& shape){
	Box box;
	bg::envelope(shape.getMultiP(), box);
	
	// Translation to bin origin
	translate<Shape>(shape, -box.min_corner().x() + Parser::getDims().x()/2,
							-box.min_corner().y() + Parser::getDims().y() * _binNumber * SPACE_COEF);
}




void Freeze::gravityMoveDown(Shape& shape){
    translate<Shape>(shape, 0., GRAVITY_STEP);
}

void Freeze::wiggle(Shape& shape){
	float randomX, randomY, randomAngle;
	
	// First bin randoms can be: -1,0,1
	// Then the shape can fly upwards
	randomX = rand()%101 - 50.;
	randomY = rand()%3 - 1. - (_binNumber/5.);
	randomAngle = rand()%3 -1.;
	
	translate<Shape>(shape, randomX, randomY);
	rotate<Shape>(shape, randomAngle);
}


void Freeze::bounceUp(Shape& shape){
    translate<Shape>(shape, 0., -BOUNCE_STEP);
    wiggle(shape);
}

void Freeze::bounceLeft(Shape& shape){
    translate<Shape>(shape, -BOUNCE_STEP, 0.);
    wiggle(shape);
}

void Freeze::bounceRight(Shape& shape){
    translate<Shape>(shape, BOUNCE_STEP, 0.);
    wiggle(shape);
}

void Freeze::bounceShape(Shape& shapeMove, Shape& shapeStill){
    Box boxM, boxS;

    bg::envelope(shapeMove.getMultiP(), boxM);
    bg::envelope(shapeStill.getMultiP(), boxS);

    int diffX = (boxS.max_corner().x() - boxS.min_corner().x())/2 - (boxM.max_corner().x() - boxM.min_corner().x())/2;
    int diffY = (boxS.max_corner().y() - boxS.min_corner().y())/2 - (boxM.max_corner().y() - boxM.min_corner().y())/2;

    translate<Shape>(shapeMove, -REPEL_STEP_MULTIPLE*diffX, -REPEL_STEP_MULTIPLE*diffY);
    
    wiggle(shapeMove);
}



