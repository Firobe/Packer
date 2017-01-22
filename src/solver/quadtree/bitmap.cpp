#include <vector>
#include <iostream>

#include <boost/geometry.hpp>
// Plante sans cette include pour une raison indeterminé

#include <boost/geometry/algorithms/envelope.hpp>
#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/io/wkt/write.hpp>
#include <boost/geometry/algorithms/intersects.hpp>
#include <boost/geometry/algorithms/intersection.hpp>

#include "bitmap.hpp"
#include "common.hpp"
#include "Shape.hpp"


bitmap::bitmap(Shape &shape, int width, int height) : width(width), height(height) {

	map = new bool[height*width];
	for(int i=0; i<width*height; i++)
		map[i] = false;

	// Compute the shape Box envelop
	Box envelop;
	bg::envelope(shape.getMultiP(), envelop);
	bg::correct(envelop);
	Point reference = envelop.min_corner();

	// Place the shape into the (0,0) point in order to create the quadTree
	translate<Shape>(shape, -reference.x(), -reference.y());
	translate<Box>(envelop, -reference.x(), -reference.y());


	double xpres = (double) envelop.max_corner().x()/width;
	double ypres = (double) envelop.max_corner().y()/height;

	std::cout << "X : " << width << " cells, " << envelop.max_corner().x() << " width, " << xpres << " /cell" << std::endl;
	std::cout << "Y : " << height << " cells, " << envelop.max_corner().y() << " height, " << ypres << " /cell" << std::endl;

	MultiPolygon mult = shape.getMultiP();
	// Detect collisions with the grid on the x axis
	int nbBlack = 0;
	for (int y=1; y<height; y++) {
		Line line{{0,y*ypres},{envelop.max_corner().x(), y*ypres}};
		std::vector<Line> line_v;
		bg::intersection(line, mult, line_v);
		for (Line& line_r : line_v) {
			double x1 = line_r[0].x(), x2 = line_r[1].x();
			for (int x=(int) x1/xpres; x<=(int) x2/xpres; x++) {
				if (!get(x,y-1)) {
					nbBlack++;
					set(x,y-1,true);
				}
				if (!get(x,y)) {
					nbBlack++;
					set(x,y,true);
				}
			}
		}
	}

	// Detect collision with the grid on the y axis
	for (int x=1; x<width; x++) {
		Line line{{x*xpres,0},{x*xpres, envelop.max_corner().y()}};
		std::vector<Line> line_v;
		bg::intersection(line, mult, line_v);
		for (Line& line_r : line_v) {
			double y1 = line_r[0].y(), y2 = line_r[1].y();
			for (int y=(int) y1/ypres; y<=(int) y2/ypres; y++) {
				if (!get(x-1,y)) {
					nbBlack++;
					set(x-1,y,true);
				}
				if (!get(x,y)) {
					nbBlack++;
					set(x,y,true);
				}
			}
		}
	}


	std::cout << nbBlack << std::endl;

	// Restore shape position
	translate<Shape>(shape, reference.x(), reference.y());
}

void bitmap::set(int i, int j, bool val) {
	map[width*i+j]=val;
}

bool bitmap::get(int i, int j) {
	return map[width*i+j];
}
