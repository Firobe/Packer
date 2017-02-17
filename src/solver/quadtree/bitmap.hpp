#ifndef BITMAP_HPP
#define BITMAP_HPP

#include <string>

#include "Shape.hpp"

/**
 * @brief The bitmap class is used in order to rasterize a Shape or a MultiPolygon.
 * The genererated bitmap still can be modified after this
 */
class bitmap
{
private:
	bool* map;
	int width;
	int height;
	int nbBlack;
	int _offsetX, _offsetY;

private:
	void copy(const bitmap&);
public:
	bitmap(const bitmap&); //copy operator
	bitmap& operator=(const bitmap&); //assignment operator
	~bitmap();

private:
	void construct(MultiPolygon &mult, int, float, float, int, float, float);
public:
	bitmap(MultiPolygon &mult, float pres);
	bitmap(Shape &s, float pres);
	bitmap(MultiPolygon &mult, int width, int height);
	bitmap(Shape &s, int width, int height);
	bitmap(const bool* nmap, int width, int height);
	bitmap(int width, int height);

	void set(int x, int y, bool);
	bool get(int x, int y) const;

	int getWidth() { return width; }
	int getHeight() { return height; }

	static bitmap *rotate(const bitmap *other, float r, int &xres, int &yres);
	static bitmap *trim(const bitmap *bmp, int &xres, int &yres);

	bool hasBlack(int offsetX, int offsetY, int length);
	bool hasBlack(int offsetX, int offsetY, int lengthX, int lengthY);
	bool hasWhite(int offsetX, int offsetY, int length);
	bool hasWhite(int offsetX, int offsetY, int lengthX, int lengthY);

	bool intersects(const bitmap& bmap) const;

	void saveMap(std::string filename);

public:
	friend std::ostream& operator<<(std::ostream& s, const bitmap&);
};

#endif // BITMAP_HPP
