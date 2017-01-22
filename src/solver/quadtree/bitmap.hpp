#ifndef BITMAP_HPP
#define BITMAP_HPP

#include "Shape.hpp"

class bitmap
{
private:
	bool* map;
	int width;
	int height;

public:
	bitmap(Shape &s, int width, int height);
	void set(int, int, bool);
	bool get(int, int);
};

#endif // BITMAP_HPP
