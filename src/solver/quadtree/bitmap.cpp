#include <vector>
#include <iostream>
#include <fstream>
#include <string>

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

using namespace std;

void bitmap::copy(const bitmap &bmap) {
	width = bmap.width;
	height = bmap.height;
	nbBlack = bmap.nbBlack;
	if (map != nullptr)
		delete [] map;
	map = new bool[height*width];
	for (int i = 0; i < width*height; i++)
		map[i] = bmap.map[i];
}

bitmap::bitmap(const bitmap &q) {
	copy(q);
}

bitmap &bitmap::operator=(const bitmap &q) {
	if (this != &q) {
		this->~bitmap();
		copy(q);
	}
	return *this;
}

bitmap::~bitmap() {
	if (map != nullptr) delete [] map;
	map = nullptr;
}


bitmap::bitmap(MultiPolygon &mult, int width, int height) : width(width), height(height) {

	map = new bool[height*width];
	for(int i=0; i<width*height; i++)
		map[i] = false;

	// Compute the shape Box envelop
	Box envelop;
	bg::envelope(mult, envelop);
	bg::correct(envelop);
	Point reference = envelop.min_corner();

	// Place the shape into the (0,0) point in order to create the quadTree
	translate<MultiPolygon>(mult, -reference.x(), -reference.y());
	translate<Box>(envelop, -reference.x(), -reference.y());


	double xpres = (double) envelop.max_corner().x()/width;
	double ypres = (double) envelop.max_corner().y()/height;

	std::cout << "X : " << width << " cells, " << envelop.max_corner().x() << " width, " << xpres << " /cell" << std::endl;
	std::cout << "Y : " << height << " cells, " << envelop.max_corner().y() << " height, " << ypres << " /cell" << std::endl;

	// Detect collisions with the grid on the x axis
	int nbBlack = 0;
	for (int y=1; y<height; y++) {
		Line line{{0,y*ypres},{envelop.max_corner().x(), y*ypres}};
		std::vector<Line> line_v;
		bg::intersection(line, mult, line_v);
		for (Line& line_r : line_v) {
			double x1 = line_r[0].x();
			bool first = true;
			for (Point& p : line_r) {
				if (!first) {
					double x2 = p.x();
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
					x1 = x2;
				}
				else
					first = false;
			}
		}
	}

	// Detect collision with the grid on the y axis
	for (int x=1; x<width; x++) {
		Line line{{x*xpres,0},{x*xpres, envelop.max_corner().y()}};
		std::vector<Line> line_v;
		bg::intersection(line, mult, line_v);
		for (Line& line_r : line_v) {
			//cout << bg::wkt(line_r) << " ";
			double y1 = line_r[0].y();
			bool first = true;
			for (Point& p : line_r) {
				if (!first) {
					double y2 = p.y();
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
					y1 = y2;
				}
				else
					first = false;
			}
		}
		//cout << endl;
	}

	this->nbBlack = nbBlack;
	std::cout << nbBlack << " bits occuped" << std::endl;

	// Restore shape position
	translate<MultiPolygon>(mult, reference.x(), reference.y());
}

bitmap::bitmap(Shape &shape, int width, int height) : bitmap(shape.getMultiP(), width, height) {}

void bitmap::set(int i, int j, bool val) {
	map[width*i+j]=val;
}

bool bitmap::get(int i, int j) {
	return map[width*i+j];
}

/**
 * @brief bitmap::hasBlack test if there is a black point in the square
   (offsetX, offsetY) - (offsetX+length, offsetY+length) of the bitmap
 * @param offsetX
 * @param offsetY
 * @param length
 */
bool bitmap::hasBlack(int offsetX, int offsetY, int length) {
	//if (2*nbBlack < width*height) return !hasWhite(offsetX, offsetY, length);

	// We first try first and last line for a better execution time in the majority of the case
	// We don't try first and last row because of the memory order of the map
	// We don't use get to accelerate the processing (especially to facilitate memoty caching of lines)

	// Two loop for memory caching
	int offset = offsetX*width;
	for (int y = offsetY; y < offsetY+length; y++) {
		if (map[offset + y])
			return true;
	}

	offset = (offsetX+length-1)*width;
	for (int y = offsetY; y < offsetY+length; y++) {
		if (map[offset + y])
			return true;
	}

	// Now we try other values
	// We iterate throught x first because of the memory placement of elements in the map
	for (int x = offsetX+1; x < offsetX+length-1; x++) {
		offset = x*width;
		for (int y = offsetY; y < offsetY+length; y++) {
			if (map[offset+y])
				return true;
		}
	}

	// False if no white dot found
	return false;
}

/**
 * @brief bitmap::hasBlack test if there is a white point in the square
   (offsetX, offsetY) - (offsetX+length, offsetY+length) of the bitmap
 * @param offsetX
 * @param offsetY
 * @param length
 */
bool bitmap::hasWhite(int offsetX, int offsetY, int length) {
	//if (2*nbBlack > width*height) return !hasBlack(offsetX, offsetY, length);

	// We first try first and last line for a better execution time in the majority of the case
	// We don't try first and last row because of the memory order of the map
	// We don't use get to accelerate the processing (especially to facilitate memoty caching of lines)

	// Two loop for memory caching
	int offset = offsetX*width;
	for (int y = offsetY; y < offsetY+length; y++) {
		if (!map[offset + y])
			return true;
	}

	offset = (offsetX+length-1)*width;
	for (int y = offsetY; y < offsetY+length; y++) {
		if (!map[offset + y])
			return true;
	}

	// Now we try other values
	// We iterate throught x first because of the memory placement of elements in the map
	for (int x = offsetX+1; x < offsetX+length-1; x++) {
		offset = x*width;
		for (int y = offsetY; y < offsetY+length; y++) {
			if (!map[offset+y])
				return true;
		}
	}

	// If all points are black, there is no white point
	return false;
}

// save the bitmap in the PBM format
// Simple picture format that don't need another lib
void bitmap::saveMap(string filename) {
	ofstream file;
	file.open(filename + ".pbm");
	file << "P1" << endl;
	file << width << " " << height << endl;

	int offset;
	for (int x = 0; x < width; x++) {
		offset = x*width;
		for (int y = 0; y < height; y++) {
			if (map[offset+y])
				file << "1";
			else
				file << "0";
			if (y!= height-1)
				file << " ";
		}
		file << endl;
	}

	file.close();

}
