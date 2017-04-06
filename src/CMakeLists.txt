add_library(packer_lib STATIC
    Parser.cpp
    Shape.cpp
    Matrix.cpp
    Splitter.cpp
    Outer.cpp
    Merger.cpp
    Interpolator.cpp
    Display.cpp
    CloseEnough.cpp
	Layout.cpp
	quadtree/QuadTree.cpp
    quadtree/InnerQuadTree.cpp
	quadtree/bitmap.cpp
    transformer/SimpleTransformer.cpp
    transformer/HoleTransformer.cpp
    solver/Solver.cpp
	solver/LineSolver.cpp
	solver/MultilineSolver.cpp
	solver/FreezeSolver.cpp
	solver/ProbaSolver.cpp
	solver/TaskSolver.cpp
	solver/ScanlineSolver.cpp)

if (${DISPLAY})
	find_package(SFML 2.0 REQUIRED system window graphics)
	target_link_libraries(packer_lib ${SFML_LIBRARIES})
endif ()

