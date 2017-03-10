add_library(packer_lib STATIC
  Parser.cpp
  Shape.cpp
  Solver.cpp
  solver/box/ToInfinityAndBeyond.cpp
  solver/box/TheSkyIsTheLimit.cpp
  solver/box/Freeze.cpp
  solver/quadtree/quadTree.cpp
  Outer.cpp
  Matrix.cpp
  solver/box/Scanline.cpp
  Splitter.cpp
  Merger.cpp
  Interpolator.cpp
  SimpleTransformer.cpp
  Display.cpp
  CloseEnough.cpp
  )

if (${ENABLE_DISPLAY})
	find_package(SFML 2.0 REQUIRED system window graphics)
	target_link_libraries(packer_lib ${SFML_LIBRARIES})
endif ()
