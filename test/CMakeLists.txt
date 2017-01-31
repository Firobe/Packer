configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vertebrae.svg ${CMAKE_CURRENT_BINARY_DIR}/vertebrae.svg @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test.svg ${CMAKE_CURRENT_BINARY_DIR}/test.svg @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/simple.svg ${CMAKE_CURRENT_BINARY_DIR}/simple.svg @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/lots.svg ${CMAKE_CURRENT_BINARY_DIR}/lots.svg @ONLY)

add_custom_target(run_ToInfinityAndBeyondSolver
        COMMAND packer --dup=false ${CMAKE_SOURCE_DIR}/examples/vertebrae.svg > output.svg
        COMMAND firefox output.svg)

#add_test(NAME test_ToInfinityAndBeyondSolver
#        COMMAND packer ${CMAKE_SOURCE_DIR}/examples/vertebrae.svg > output.svg
#        )

add_executable(splitter SplitterTest.cpp)
target_link_libraries(splitter utils_lib)
target_link_libraries(splitter solver_lib)
add_test(SplitterTest splitter)

add_executable(parser ParserTest.cpp)
target_link_libraries(parser parser_lib)
target_link_libraries(parser utils_lib)
add_test(ParserTest parser)

add_executable(compare Compare.cpp)
target_link_libraries(compare utils_lib)
target_link_libraries(compare solver_lib)

add_executable(quadtree quadtreeTest.cpp)
target_link_libraries(quadtree parser_lib)
target_link_libraries(quadtree utils_lib)
target_link_libraries(quadtree solver_lib)

add_executable(bitmap bitmapTest.cpp)
target_link_libraries(bitmap utils_lib)
add_test(bitmapTest bitmap)
# add_executable(openmp openmp.cpp)
