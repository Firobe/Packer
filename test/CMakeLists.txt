configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vertebrae.svg ${CMAKE_CURRENT_BINARY_DIR}/vertebrae.svg @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test.svg ${CMAKE_CURRENT_BINARY_DIR}/test.svg @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/simple.svg ${CMAKE_CURRENT_BINARY_DIR}/simple.svg @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/lots.svg ${CMAKE_CURRENT_BINARY_DIR}/lots.svg @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mergeGood.svg ${CMAKE_CURRENT_BINARY_DIR}/mergeGood.svg @ONLY)


add_executable(splitter SplitterTest.cpp)
target_link_libraries(splitter packer_lib)
add_test(SplitterTest splitter)

add_executable(parser ParserTest.cpp)
target_link_libraries(parser packer_lib)
add_test(ParserTest parser)

add_executable(compare Compare.cpp)
target_link_libraries(compare packer_lib)
add_test(CompareTest compare)

add_executable(quadtree quadtreeTest.cpp)
target_link_libraries(quadtree packer_lib)
add_test(quadTreeTest quadtree)

add_executable(interpolator interpolatorTest.cpp)
target_link_libraries(interpolator packer_lib)
add_test(InterpolatorTest interpolator)

add_executable(bitmap bitmapTest.cpp)
target_link_libraries(bitmap packer_lib)
add_test(bitmapTest bitmap)

#add_executable(openmp openmp.cpp)
