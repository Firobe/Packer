#include <string>
#include <stdexcept>

#include "solver/quadtree/bitmap.hpp"
#include "commonTest.hpp"

using namespace std;

int main() {
	bool nmap[] = {true, true, false, false,
				   true, true, true, false,
				   true, true, false, false,
				   false, false, true, true};
	bitmap bmap(nmap, 4, 4);
	bmap.saveMap("bmap-test-1");

	// Testing map values
	ASSERT(bmap.get(0,0) == true, "bad value at pixel (0,0)");
	ASSERT(bmap.get(1,0) == true, "bad value at pixel (1,0)");
	ASSERT(bmap.get(2,0) == false, "bad value at pixel (2,0)");
	ASSERT(bmap.get(3,0) == false, "bad value at pixel (3,0)");
	ASSERT(bmap.get(0,1) == true, "bad value at pixel (0,1)");
	ASSERT(bmap.get(1,1) == true, "bad value at pixel (1,1)");
	ASSERT(bmap.get(2,1) == true, "bad value at pixel (2,1)");
	ASSERT(bmap.get(3,1) == false, "bad value at pixel (3,1)");
	ASSERT(bmap.get(0,2) == true, "bad value at pixel (0,2)");
	ASSERT(bmap.get(1,2) == true, "bad value at pixel (1,2)");
	ASSERT(bmap.get(2,2) == false, "bad value at pixel (2,2)");
	ASSERT(bmap.get(3,2) == false, "bad value at pixel (3,2)");
	ASSERT(bmap.get(0,3) == false, "bad value at pixel (0,3)");
	ASSERT(bmap.get(1,3) == false, "bad value at pixel (1,3)");
	ASSERT(bmap.get(2,3) == true, "bad value at pixel (2,3)");
	ASSERT(bmap.get(3,3) == true, "bad value at pixel (3,3)");
	bmap.set(2,2,true);
	ASSERT(bmap.get(2,2) == true, "bad value set at pixel (2,2)");
	bmap.set(2,2,false);
	ASSERT(bmap.get(2,2) == false, "bad value set at pixel (2,2)");


	// Testing hasWhite
	ASSERT(bmap.hasWhite(0,0,2) == false, "hasWhite(0,0,2)");
	ASSERT(bmap.hasWhite(0,0,2,3) == false, "hasWhite(0,0,2,3)");
	ASSERT(bmap.hasWhite(0,0,3,2) == true, "hasWhite(0,0,3,2)");
	ASSERT(bmap.hasWhite(0,0,4) == true, "hasWhite(0,0,4)");
	ASSERT(bmap.hasWhite(2,3,2,1) == false, "hasWhite(2,3,2,0)");
	ASSERT(bmap.hasWhite(1,1,2) == true, "hasWhite(1,1,2)");
	ASSERT(bmap.hasWhite(1,2,1) == false, "hasWhite(1,2,1)");
	ASSERT(bmap.hasWhite(0,1,2,1) == false, "hasWhite(0,1,2,1)");


	// Testing hasBlack
	ASSERT(bmap.hasBlack(0,0,2,3) == true, "hasBlack(0,0,2,3)");
	ASSERT(bmap.hasBlack(2,3,1) == true, "hasBlack(2,3,1)");
	ASSERT(bmap.hasBlack(3,2,1) == false, "hasBlack(3,2,1)");
	ASSERT(bmap.hasBlack(1,2,2) == true, "hasBlack(1,2,2)");
	ASSERT(bmap.hasBlack(3,0,1,3) == false, "hasBlack(3,0,1,3)");
	ASSERT(bmap.hasBlack(2,2,2,1) == false, "hasBlack(2,2,2,1)");

}
