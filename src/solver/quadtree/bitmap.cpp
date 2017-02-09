#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

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
	cout << "bitmap copied" << endl;
	_offsetX=bmap._offsetX;
	_offsetY=bmap._offsetY;
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


void bitmap::construct(MultiPolygon &mult, int width, float xpres, float xmax, int height, float ypres, float ymax) {
	nbBlack = 0;

	// Detect collisions with the grid on the x axis
	for (int y=1; y<height; y++) {
		Line line{{0,y*ypres},{xmax, y*ypres}};
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
		Line line{{x*xpres,0},{x*xpres, ymax}};
		std::vector<Line> line_v;
		bg::intersection(line, mult, line_v);
		for (Line& line_r : line_v) {
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
	}

}


bitmap::bitmap(MultiPolygon &mult, float pres) {
	// Compute the shape Box envelop
	Box envelop;
	bg::envelope(mult, envelop);
	bg::correct(envelop);
	Point reference = envelop.min_corner();

	// Place the shape into the (0,0) point in order to create the quadTree
	translate<MultiPolygon>(mult, -reference.x(), -reference.y());
	translate<Box>(envelop, -reference.x(), -reference.y());

	// Determine the bitmap size according to this information
	width = ((int) envelop.max_corner().x()/pres + 1);
	height = ((int) envelop.max_corner().y()/pres + 1);

	map = new bool[height*width];
	for(int i=0; i<width*height; i++)
		map[i] = false;

	// Determine the bitmap position
	// Poisition is a approximation and is asumed as descritized with a pres step
	_offsetX = (int) reference.x()/pres;
	_offsetY = (int) reference.y()/pres;

	construct(mult,
			  width, (float) pres, envelop.max_corner().x(),
			  height, (float) pres, envelop.max_corner().y());

	// Restore shape position
	translate<MultiPolygon>(mult, reference.x(), reference.y());
}

bitmap::bitmap(Shape &shape, float pres) : bitmap(shape.getMultiP(), pres) {}

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

	construct(mult,
			  width, xpres, envelop.max_corner().x(),
			  height, ypres, envelop.max_corner().y());

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
 * @brief bitmap::bitmap construct an empty bitmap
 * @param width
 * @param height
 */
bitmap::bitmap(int width, int height) : width(width), height(height) {
	map = new bool[width*height];
	for (int i=0; i<width*height; i++) {
		map[i] = false;
	}
	nbBlack = 0;
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
bool bitmap::get(int x, int y) const {
	if (x < 0 || x > width-1 || y < 0 || y > height-1)
		return false;
	return map[BMAP_COORD(x,y)];
}


bitmap *bitmap::rotate(const bitmap *bmap, float r)
{
	r = -r;

	float w = bmap->width;
	float h = bmap->height;

	int aX = (int)ceil(w*cos(r) - h*sin(r));
	int aY = (int)ceil(w*sin(r) + h*cos(r));

	int bX = (int)ceil(-h*sin(r));
	int bY = (int)ceil(h*cos(r));

	int cX = (int)ceil(w*cos(r));
	int cY = (int)ceil(w*sin(r));

	int xMin = min(min(0, aX), min(bX, cX));
	int xMax = max(max(0, aX), max(bX, cX));
	int yMin = min(min(0, aY), min(bY, cY));
	int yMax = max(max(0, aY), max(bY, cY));

	int width = xMax-xMin;
	int height = yMax-yMin;

	float oldCenterX = w/2;
	float oldCenterY = h/2;
	float centerX = width/2;
	float centerY = height/2;

	float cosr = cos(r);
	float sinr = sin(r);

	bitmap *rotated = new bitmap(width, height);
	for (int x=0; x<width; x++) {
		for (int y=0; y<height; y++) {
			int cX = round(x-centerX);
			int cY = round(y-centerY);
			int X = round((cosr*cX - sinr*cY) + oldCenterX);
			int Y = round((sinr*cX + cosr*cY) + oldCenterY);
			rotated->set(x, y, bmap->get(X, Y));
			}
	}

	return rotated;
}


bitmap *bitmap::trim(const bitmap *bmp)
{
	bool found = false;
	int minX=0, minY=0;
	int maxX=0, maxY=0;

	for (int x=0; x<bmp->width; x++) {
		for (int y=0; y<bmp->height; y++) {
			if (bmp->get(x, y)) {
				if (!found) {
					found = true;
					minX = maxX = x;
					minY = maxY = y;
				} else {
					if (x < minX) minX = x;
					if (y < minY) minY = y;
					if (x > maxX) maxX = x;
					if (y > maxY) maxY = y;
				}
			}
		}
	}

	int deltaX = maxX-minX;
	int deltaY = maxY-minY;
	bitmap *trimmed = new bitmap(deltaX, deltaY);
	for (int x=0; x<deltaX; x++) {
		for (int y=0; y<deltaY; y++) {
			trimmed->set(x, y, bmp->get(x+minX, y+minY));
		}
	}

	return trimmed;
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

bool bitmap::intersects(const bitmap& bmap) const {
	// We first search if global size intersect or not for a faster computation
	if (_offsetX+width < bmap._offsetX || bmap._offsetX+bmap.width  < _offsetX)
		return false;
	if (_offsetY+height < bmap._offsetY || bmap._offsetY+bmap.height  < _offsetY)
		return false;

	int offsetX = bmap._offsetX - _offsetX;
	int offsetY = bmap._offsetY - _offsetY;

	for (int x=0; x<width; x++) {
		for (int y=0; y<height; y++) {
			if (get(x, y) && bmap.get(x+offsetX, y+offsetY)) {
				return true;
			}
		}
	}
	return false;
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
