cmake_minimum_required(VERSION 2.6)
project(free_packingPFA)

enable_testing()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wfatal-errors -std=c++11 -O2")

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
add_subdirectory(src)
add_subdirectory(test)

add_executable(packer src/main.cpp)
target_link_libraries(packer packer_lib)


# Use Astar to reformat source files
add_custom_target(format
        COMMAND astyle -RA2fpHUxC90xLyjk1W1 \"src/*.cpp\" \"src/*.hpp\"
        COMMAND rm -Rf src/*.orig
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
