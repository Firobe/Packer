#include <boost/geometry/algorithms/envelope.hpp>
#include <vector>


#include "Freeze.hpp"
#include "Log.hpp"
#include "common.hpp"
#include "Display.hpp"
#include "types.hpp"

#define PHYSICS_GRAVITY_TIME        500
#define PHYSICS_STABILIZATION_TIME  500
#define PHYSICS_ALL_TIME			(PHYSICS_GRAVITY_TIME + PHYSICS_STABILIZATION_TIME)
#define GRAVITY_STEP                10.
#define BOUNCE_STEP                 10.
#define REPEL_STEP_MULTIPLE			1.


using namespace std;

// Principe: On balance une pièce au dessus
void FreezeSolver::solveBin(){
    Box box;

    for (unsigned i = 0 ; i < _shapes.size(); ++i) {

        randomStartingPoint(_shapes[i]);
        randomStartingRotation(_shapes[i]);

        for(unsigned t = 0 ; t < PHYSICS_ALL_TIME ; ++t){
            bg::envelope(_shapes[i], box);

            if(boxA.max_corner().y() > Parser::getDims().y()){
                bounceUp(_shapes[i]); // Move up + random slight rotation
            } else if(boxA.min_corner().x() < 0){
                bounceRight(_shapes[i]);
            } else if(boxA.max_corner().x() > Parser::getDims().x()){
                bounceLeft(_shapes[i]);
            } else{
                for(unsigned j = 0 ; j < i ; ++j){
                    if(bg::intersects(_shapes[i].getMultiP(), _shapes[j].getMultiP())){
                        bounceShape(_shapes[i],_shapes[j]);
                    }
                }

                if(t < PHYSICS_GRAVITY_TIME){
					gravityMoveDown(_shapes[i]);
                }

            }
        }

        // Checking if stabilization worked, else shape is sent to next bin
        bool stabilized = true;
        
        bg::envelope(_shapes[i], box);
        if(box.min_corner.x() < 0 || box.min_corner.x() > Parser::getDims().x() ||
           box.min_corner.y() < 0 || box.min_corner.y() > Parser::getDims().y()){
			stabilized = false;
		   }
		if(stabilized != false){
			for(unsigned j = 0 ; j < i ; ++j){
				if(bg::intersects(_shapes[i].getMultiP(), _shapes[j].getMultiP())){
					stabilized = false;
					break;
                }
			}
		}
		
		if(stabilized == true){
			markPacked(i);
		}
    }
}

void FreezeSolver::randomStartingPoint(Shape shape){
	Box box;
	bg::envelope(shape, box);
	
	// Translation to bin origin
	translate<Shape>(shape, -box.min_corner().x(),
							-box.min_corner().y() + Parser::getDims().y() * _binNumber * SPACE_COEF);
	
	
}




void FreezeSolver::gravityMoveDown(Shape shape){
    translate<Shape>(shape, 0., GRAVITY_STEP);
}

void FreezeSolver::wiggle(Shape shape){
	float randomX, randomY, randomAngle;
	
	translate<Shape>(shape, randomX, randomY);
	rotate<Shape>(shape, randomAngle);
}


void FreezeSolver::bounceUp(Shape shape){
    translate<Shape>(shape, 0., -BOUNCE_STEP);
    wiggle(shape);
}

void FreezeSolver::bounceLeft(Shape shape){
    translate<Shape>(shape, -BOUNCE_STEP, 0.);
    wiggle(shape);
}

void FreezeSolver::bounceRight(Shape shape){
    translate<Shape>(shape, BOUNCE_STEP, 0.);
    wiggle(shape);
}

void FreezeSolver::bounceShape(Shape shapeMove, Shape shapeStill){
    Box boxM, boxS;

    bg::envelope(shapeMove, boxM);
    bg::envelope(shapeStill, boxS);

    int diffX = (boxS.max_corner.x() - boxS.min_corner.x())/2 - (boxM.max_corner.x() - boxM.min_corner.x())/2;
    int diffY = (boxS.max_corner.y() - boxS.min_corner.y())/2 - (boxM.max_corner.y() - boxM.min_corner.y())/2;

    translate<Shape>(shapeMove, REPEL_STEP_MULTIPLE*diffX, REPEL_STEP_MULTIPLE*diffY);
    
    wiggle(shape);
}



