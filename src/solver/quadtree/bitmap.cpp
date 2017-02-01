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

#define BMAP_COORD(X,Y) width*Y + X

using namespace std;


/**
 * @brief bitmap::copy copy the content of bmap into this
 * @param bmap
 */
void bitmap::copy(const bitmap &bmap) {
	width = bmap.width;
	height = bmap.height;
	nbBlack = bmap.nbBlack;
	map = new bool[height*width];
	for (int i = 0; i < width*height; i++)
		map[i] = bmap.map[i];
}


/**
 * @brief copy constructor
 * @param q
 */
bitmap::bitmap(const bitmap &q) {
	copy(q);
}


/**
 * @brief assignement operator
 * @param bmap
 * @return the bitmap copied
 */
bitmap &bitmap::operator=(const bitmap &bmap) {
	if (this != &bmap) {
		this->~bitmap();
		copy(bmap);
	}
	return *this;
}


/**
 * @brief destructor
 */
bitmap::~bitmap() {
	if (map != nullptr) delete [] map;
	map = nullptr;
}


/**
 * @brief bitmap::bitmap generate a bitmap that is the rasterization of the boost MultiPolygon
 * @param mult MultiPolygon to rasterize
 * @param width width of the bitmap
 * @param height height of the bitmap
 */
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

	// Restore shape position
	translate<MultiPolygon>(mult, reference.x(), reference.y());
}


/**
 * @brief bitmap::bitmap alllow to construct a bitmap from a bool array.
 * Any verification is done to ensure the array has the correct size.
 * The array is copied and not modified
 * @param nmap
 * @param width
 * @param height
 */
bitmap::bitmap(const bool * nmap, int width, int height) : width(width), height(height), nbBlack(0) {
	map = new bool[width*height];
	for (int i=0; i<width*height; i++) {
		map[i] = nmap[i];
		if (map[i])
			nbBlack++;
	}
}


/**
 * @brief Constructor for Shapes, it just call the MultiPolygon constructor with the Shape's MultiPolygon
 * @param shape
 * @param width
 * @param height
 */
bitmap::bitmap(Shape &shape, int width, int height) : bitmap(shape.getMultiP(), width, height) {}


/**
 * @brief bitmap::set set the value of the point (i,j)
 * @param x absciss
 * @param y ordiante
 * @param val true : black, false : white
 */
void bitmap::set(int x, int y, bool val) {
	map[BMAP_COORD(x,y)]=val;
}


/**
 * @brief bitmap::get the value of the point (i,j)
 * @param x absciss
 * @param y ordiante
 */
bool bitmap::get(int x, int y) {
	return map[BMAP_COORD(x,y)];
}


/**
 * @brief bitmap::hasBlack test if there is a black point in the square
   (offsetX, offsetY) - (offsetX+length, offsetY+length) of the bitmap
 * @param offsetX
 * @param offsetY
 * @param length
 */
bool bitmap::hasBlack(int offsetX, int offsetY, int lengthX, int lengthY) {
	//if (2*nbBlack < width*height) return !hasWhite(offsetX, offsetY, length);

	// We first try first and last line for a better execution time in the majority of the case
	// We don't try first and last row because of the memory order of the map
	// We don't use get to accelerate the processing (especially to facilitate memoty caching of lines)

	// Two loop for memory caching
	int offset = offsetY*width;
	for (int x = offsetX; x < offsetX+lengthX; x++) {
		if (map[offset + x])
			return true;
	}

	offset = (offsetY+lengthY-1)*width;
	for (int x = offsetX; x < offsetX+lengthX; x++) {
		if (map[offset + x])
			return true;
	}

	// Now we try other values
	// We iterate throught x first because of the memory placement of elements in the map
	for (int y = offsetY+1; y < offsetY+lengthY-1; y++) {
		offset = y*width;
		for (int x = offsetX; x < offsetX+lengthX; x++) {
			if (map[offset+x])
				return true;
		}
	}

	// False if no white dot found
	return false;
}


/**
 * @brief bitmap::hasBlack test if there is a black point in the square
   (offsetX, offsetY) - (offsetX+length, offsetY+length) of the bitmap
 * @param offsetX
 * @param offsetY
 * @param length
 */
bool bitmap::hasBlack(int offsetX, int offsetY, int length) {
	return hasBlack(offsetX, offsetY, length, length);
}

/**
 * @brief bitmap::hasBlack test if there is a white point in the square
   (offsetX, offsetY) - (offsetX+lengthX, offsetY+lengthY) of the bitmap
 * @param offsetX
 * @param offsetY
 * @param lengthX
 * @param lengthY
 */
bool bitmap::hasWhite(int offsetX, int offsetY, int lengthX, int lengthY) {
	// Two loop for memory caching
	int offset = offsetY*width;
	for (int x = offsetX; x < offsetX+lengthX; x++) {
		if (!map[offset + x])
			return true;
	}

	offset = (offsetY+lengthY-1)*width;
	for (int x = offsetX; x < offsetX+lengthX; x++) {
		if (!map[offset + x])
			return true;
	}

	// Now we try other values
	// We iterate throught x first because of the memory placement of elements in the map
	for (int y = offsetY+1; y < offsetY+lengthY-1; y++) {
		offset = y*width;
		for (int x = offsetX; x < offsetX+lengthX; x++) {
			if (!map[offset+x])
				return true;
		}
	}

	// If all points are black, there is no white point
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
	return hasWhite(offsetX, offsetY, length, length);
}


/**
 * @brief bitmap::saveMap save the bitmap in the PBM format
 * @param filename base name of the file, the full name will be : filename.pbm
 */
void bitmap::saveMap(string filename) {
	ofstream file;
	file.open(filename + ".pbm");
	file << "P1" << endl;
	file << width << " " << height << endl;

	int offset;
	for (int y = 0; y < height; y++) {
		offset = y*width;
		for (int x = 0; x < width; x++) {
			if (map[offset+x])
				file << "1 ";
			else
				file << "0 ";
		}
		file << endl;
	}
	file.close();
}


/**
 * @brief operator << get informations about the bitmap for debugging purpose
 * @param s
 * @param bmap
 */
std::ostream& operator<<(std::ostream& s, const bitmap& bmap) {
	s << "Size : " << bmap.width << "x" << bmap.height << std::endl;
	s << bmap.nbBlack << " blacks out of " << bmap.width*bmap.height << " points" << std::endl;
	return s;
}
