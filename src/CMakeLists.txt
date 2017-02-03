add_library(packer_lib STATIC
  Parser.cpp
  Shape.cpp
  Solver.cpp
  solver/box/ToInfinityAndBeyond.cpp
  solver/box/TheSkyIsTheLimit.cpp
  Outer.cpp
  Matrix.cpp
  solver/box/Scanline.cpp
  Splitter.cpp
  Merger.cpp
  common.cpp
  SimpleTransformer.cpp
  )
