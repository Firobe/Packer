cmake_minimum_required(VERSION 2.6)
project(free_packingPFA)
set(BOOST_LIBS program_options)
set(LIBS_DIR third_party/boost/stage/lib/)
set(BOOST_REQUIRED_VERSION 1.62.0)
set(EXE_NAME "packer")
set(FORCE_UNPACK 0) #Set to 1 to force local installation of Boost (ensure that it will take priority)
set(ENABLE_DISPLAY 0)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

if (NOT (DEFINED target-os AND target-os STREQUAL "windows"))
    enable_testing()
endif()

if(${ENABLE_DISPLAY})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_DISPLAY")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wfatal-errors -std=c++11 -O3 -fopenmp")
if(CMAKE_BUILD_TYPE STREQUAL "coverage")
  SET(CMAKE_CXX_FLAGS "-g -O0 -pg -fprofile-arcs -ftest-coverage")
  SET(CMAKE_C_FLAGS "-g -O0 -pg -fprofile-arcs -ftest-coverage")
  if(CMAKE_COMPILER_IS_GNUCXX)
    include(CodeCoverage)
    setup_init_coverage()
    coverage_all(coverage)
  endif()
endif()

# Define where headers only libs and statics libs are located
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party)
include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(Boost ${BOOST_REQUIRED_VERSION} COMPONENTS ${BOOST_LIBS})
if(NOT Boost_FOUND OR ${FORCE_UNPACK})
	find_package(Boost)
	if(Boost_FOUND AND NOT ${FORCE_UNPACK})
		message(FATAL_ERROR "You have an out-of-date installation of Boost. Please upgrade it or completely remove it ! (or FORCE_UNPACK if you know what you are doing)")
	else ()
		message(STATUS "You have no version of Boost installed. Will locally install one.")
	endif ()
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
			if (NOT EXISTS ${CMAKE_SOURCE_DIR}/third_party/boost/stage/lib/libboost_${BOOST_LIB}-mt-s.a)
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
				message(STATUS "Done")
			endif ()
		endforeach ()
	endif ()
	include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party/boost)
	link_directories(${CMAKE_SOURCE_DIR}/${LIBS_DIR})
else ()
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
endif ()

add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(test)
add_executable(packer src/main.cpp)

target_link_libraries(packer packer_lib)
if (${ENABLE_DISPLAY})
	target_link_libraries(packer ${SFML_LIBRARIES})
endif ()

if (DEFINED target-os AND target-os STREQUAL "windows")
    message(STATUS "cross compilation")
    target_link_libraries(${EXE_NAME} boost_program_options-mt-s)
    set(EXE_NAME_PLUGIN "${EXE_NAME}.exe")
else ()
    message(STATUS "Classic compilation")
    target_link_libraries(${EXE_NAME} boost_program_options)
    set(EXE_NAME_PLUGIN "${EXE_NAME}")
    install(TARGETS ${EXE_NAME} RUNTIME
            DESTINATION ~/.config/inkscape/extensions/
            PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE)
    install(FILES ${CMAKE_BINARY_DIR}/packer.inx
            DESTINATION ~/.config/inkscape/extensions/
            PERMISSIONS OWNER_READ OWNER_WRITE)
    install(FILES ${CMAKE_BINARY_DIR}/packer.py
            DESTINATION ~/.config/inkscape/extensions/
            PERMISSIONS OWNER_READ OWNER_WRITE)
	install(FILES ${CMAKE_SOURCE_DIR}/font.ttf
			DESTINATION ~/.config/inkscape/extensions/
			PERMISSIONS OWNER_READ OWNER_WRITE)
	install(FILES ${CMAKE_SOURCE_DIR}/conf/default.ce
			DESTINATION ~/.config/inkscape/extensions/
			PERMISSIONS OWNER_READ OWNER_WRITE)
endif ()
configure_file(${CMAKE_SOURCE_DIR}/extensions/packer.inx ${CMAKE_BINARY_DIR}/packer.inx @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/extensions/packer.py ${CMAKE_BINARY_DIR}/packer.py @ONLY)
file(COPY ${CMAKE_SOURCE_DIR}/font.ttf DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/conf/default.ce DESTINATION ${CMAKE_BINARY_DIR})

# Use Astyle to reformat source files
add_custom_target(format
        COMMAND astyle -RA2fpHUxC90xLyxjxek1W1 \"src/*.cpp\" \"src/*.hpp\"
        COMMAND rm -Rf src/*.orig
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
