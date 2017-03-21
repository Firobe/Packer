add_library(packer_lib STATIC
    Parser.cpp
    Shape.cpp
    Matrix.cpp
    Splitter.cpp
    Outer.cpp
    Merger.cpp
    Interpolator.cpp
    SimpleTransformer.cpp
    HoleTransformer.cpp
    Display.cpp
    CloseEnough.cpp
	#solver/quadtree/QuadTree.cpp
    solver/quadtree/InnerQuadTree.cpp
	#solver/quadtree/bitmap.cpp
    Solver.cpp
    solver/box/ToInfinityAndBeyond.cpp
    solver/box/TheSkyIsTheLimit.cpp
	solver/box/Freeze.cpp
    solver/box/Scanline.cpp)

if (${ENABLE_DISPLAY})
	find_package(SFML 2.0 REQUIRED system window graphics)
	target_link_libraries(packer_lib ${SFML_LIBRARIES})
endif ()

