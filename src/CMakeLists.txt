add_library(parser_lib STATIC
    Parser.cpp)

add_library(utils_lib STATIC
    Shape.cpp
    Matrix.cpp
    Splitter.cpp
    Outer.cpp
    solver/quadtree/quadTree.cpp
    solver/quadtree/bitmap.cpp)

add_library(solver_lib STATIC
        Solver.cpp
        solver/box/ToInfinityAndBeyond.cpp
        solver/box/TheSkyIsTheLimit.cpp
        solver/box/Scanline.cpp)
