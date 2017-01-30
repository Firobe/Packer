#ifndef BITMAP_HPP
#define BITMAP_HPP

#include <string>

#include "Shape.hpp"

class bitmap
{
private:
	bool* map;
	int width;
	int height;
	int nbBlack;

private:
	void copy(const bitmap&);
public:
	bitmap(const bitmap&); //copy operator
	bitmap& operator=(const bitmap&); //assignment operator
	~bitmap();

	bitmap(MultiPolygon &mult, int width, int height);
	bitmap(Shape &s, int width, int height);

	void set(int, int, bool);
	bool get(int, int);
	bool hasBlack(int offsetX, int offsetY, int length);
	bool hasWhite(int offsetX, int offsetY, int length);

	void saveMap(std::string filename);

public:
	friend std::ostream& operator<<(std::ostream& s, const bitmap&);
};

#endif // BITMAP_HPP
