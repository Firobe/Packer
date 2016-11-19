cmake_minimum_required(VERSION 3.2)
project(free_packingPFA)
set(BOOST_LIBS program_options)
set(LIBS_DIR third_party/boost/stage/lib/)

enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wfatal-errors -std=c++11 -O2")

# Define where headers only libs and statics libs are located
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party/boost)
link_directories(${CMAKE_SOURCE_DIR}/${LIBS_DIR})

# Allow to automaticly unpack and build boost library according to build type
# It can build both libraries for classic compilation and cross compilation
if (NOT EXISTS ${CMAKE_SOURCE_DIR}/third_party/boost)
    message(STATUS "Unpacking boost...")
    execute_process(COMMAND tar -xf boost.tar.bz2
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party)
    execute_process(COMMAND mv boost_1_62_0 boost
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party)
    message(STATUS "done")
else ()
    message(STATUS "Boost directory found")
endif ()

# Check build type to detect wich boost lib should be build
if (DEFINED target-os AND target-os STREQUAL "windows")
    foreach (BOOST_LIB ${BOOST_LIBS})
        if (NOT EXISTS ${CMAKE_SOURCE_DIR}/third_party/boost/stage/lib/libboost_${BOOST_LIB}-mt-sd.a)
            message(STATUS "${BOOST_LIB} missing")
            message(STATUS "Building Boost required library...")
            execute_process(COMMAND sh bootstrap.sh --with-libraries=${BOOST_LIB}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party/boost)
            file(APPEND third_party/boost/project-config.jam "using gcc : windows : x86_64-w64-mingw32-g++ ;")
            execute_process(COMMAND ./b2 link=static runtime-link=static toolset=gcc-windows target-os=windows threadapi=win32 --layout=tagged
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party/boost)
            message(STATUS "done")
        endif ()
    endforeach ()
else ()
    foreach (BOOST_LIB ${BOOST_LIBS})
        if (NOT EXISTS ${CMAKE_SOURCE_DIR}/third_party/boost/stage/lib/libboost_${BOOST_LIB}.a)
            message(STATUS "${BOOST_LIB} missing")
            message(STATUS "Building Boost required library...")
            execute_process(COMMAND sh bootstrap.sh --with-libraries=${BOOST_LIB}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party/boost)
            execute_process(COMMAND ./b2 link=static
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party/boost)
            message(STATUS "done")
        endif ()
    endforeach ()
endif ()


add_subdirectory(src)
add_subdirectory(test)
add_executable(packer src/main.cpp)
target_link_libraries(packer packer_lib)
if (DEFINED target-os AND target-os STREQUAL "windows")
    target_link_libraries(packer boost_program_options-mt-sd.a)
    message(STATUS "cross compilation")
else ()
    target_link_libraries(packer boost_program_options)
    message(STATUS "Classic compilation")
endif ()

# Use Astar to reformat source files
add_custom_target(format
        COMMAND astyle -RA2fpHUxC90xLyjk1W1 \"src/*.cpp\" \"src/*.hpp\"
        COMMAND rm -Rf src/*.orig
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
