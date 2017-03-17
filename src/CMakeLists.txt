add_library(parser_lib STATIC
    Parser.cpp)

add_library(utils_lib STATIC
    Shape.cpp
    Matrix.cpp
    Splitter.cpp
    Outer.cpp
    Rotos.cpp
    Merger.cpp
    Interpolator.cpp
    SimpleTransformer.cpp
    HoleTransformer.cpp
    Display.cpp
    CloseEnough.cpp
    solver/quadtree/QuadTree.cpp
    solver/quadtree/InnerQuadTree.cpp
    solver/quadtree/bitmap.cpp)

add_library(solver_lib STATIC
        Solver.cpp
        solver/box/ToInfinityAndBeyond.cpp
        solver/box/TheSkyIsTheLimit.cpp
        solver/box/Scanline.cpp)

if (${ENABLE_DISPLAY})
	find_package(SFML 2.0 REQUIRED system window graphics)
	target_link_libraries(packer_lib ${SFML_LIBRARIES})
endif ()

