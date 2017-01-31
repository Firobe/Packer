#ifndef BITMAP_HPP
#define BITMAP_HPP

#include <string>

#include "Shape.hpp"

/**
 * @brief The bitmap class is used in order to rasterize a Shape or a MultiPolygon. The genererated bitmap still can be modified after this
 */
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
	bitmap(const bool* nmap, int width, int height);

	void set(int x, int y, bool);
	bool get(int x, int y);
	bool hasBlack(int offsetX, int offsetY, int length);
	bool hasBlack(int offsetX, int offsetY, int lengthX, int lengthY);
	bool hasWhite(int offsetX, int offsetY, int length);
	bool hasWhite(int offsetX, int offsetY, int lengthX, int lengthY);

	void saveMap(std::string filename);

public:
	friend std::ostream& operator<<(std::ostream& s, const bitmap&);
};

#endif // BITMAP_HPP
