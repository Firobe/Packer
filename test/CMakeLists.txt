add_custom_target(run_ToInfinityAndBeyondSolver
        COMMAND packer --dup=false ${CMAKE_SOURCE_DIR}/examples/vertebrae.svg > output.svg
        COMMAND firefox output.svg)

#add_test(NAME test_ToInfinityAndBeyondSolver
#        COMMAND packer ${CMAKE_SOURCE_DIR}/examples/vertebrae.svg > output.svg
#        )

add_executable(splitter SplitterTest.cpp)
target_link_libraries(splitter packer_lib)
add_test(SplitterTest splitter)

add_executable(parser ParserTest.cpp)
target_link_libraries(parser packer_lib)
add_test(ParserTest parser)

add_executable(compare Compare.cpp)
target_link_libraries(compare packer_lib)
