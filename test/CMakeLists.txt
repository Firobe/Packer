add_custom_target(run_ToInfinityAndBeyondSolver
        COMMAND packer --dup=false ${CMAKE_SOURCE_DIR}/examples/vertebrae.svg > output.svg
        COMMAND firefox output.svg)

#add_test(NAME test_ToInfinityAndBeyondSolver
#        COMMAND packer ${CMAKE_SOURCE_DIR}/examples/vertebrae.svg > output.svg
#        )