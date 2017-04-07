cmake_minimum_required(VERSION 2.8)
project(free_packingPFA)
set(BOOST_LIBS program_options)
set(LIBS_DIR third_party/boost/stage/lib/)
set(BOOST_REQUIRED_VERSION 1.62.0)
set(EXE_NAME "packer")

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wfatal-errors -std=c++11 -fopenmp")

# Options
option(FORCE_UNPACK "Boost unpack forcing") #Set to 1 to force local installation of Boost (ensure that it will take priority)
option(DISPLAY "Display build")
option(DEBUG "Enable debug compilation flags")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

# Testing enabling
if (NOT (DEFINED target-os AND target-os STREQUAL "windows"))
    enable_testing()
endif()

# find SFML if we are building for LINUX

if ((NOT (DEFINED target-os)) OR (DEFINED target-os AND (NOT (target-os STREQUAL "windows"))))
    find_package(SFML 2.4 COMPONENTS system window graphics network)
    if(NOT SFML_FOUND)
        # Then extract and compile the SFML
        message(STATUS "You have no version of SFML installed. Will locally install one.")
        
        if(NOT EXISTS ${CMAKE_SOURCE_DIR}/third_party/SFML)
            message(STATUS "Unpacking SFML ...")
            execute_process(COMMAND unzip SFML.zip
        		        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party)
            execute_process(COMMAND mv SFML-2.4.2 SFML
                            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party)
            message(STATUS "done")
        else()
            message(STATUS "SFML directory found")
        endif()

        message(STATUS "building libs")
        execute_process(COMMAND mkdir build
		        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party/SFML)
        execute_process(COMMAND mkdir install
		        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party/SFML)
        execute_process(COMMAND cmake -DCMAKE_INSTALL_PREFIX=../install ..
		        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party/SFML/build)
        execute_process(COMMAND make
		        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party/SFML/build)
        execute_process(COMMAND make install
		        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party/SFML/build)
        find_package(SFML 2.4 REQUIRED system window graphics network)
    endif()
endif()

include_directories(${SFML_INCLUDE_DIR})

# SFML Enabling
if(${DISPLAY})
	message(STATUS "Will build with display support")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_DISPLAY")
else ()
	message(STATUS "Will build WITHOUT display support")
endif ()

# Debug enabling (use it with cmake -DDEBUG=1 ..)
if(${DEBUG})
        message(STATUS "Will build debug configuration")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif ()

# Coverage build (use it with cmake -DCMAKE_BUILD_TYPE=coverage ..)
if(CMAKE_BUILD_TYPE STREQUAL "coverage")
  SET(CMAKE_CXX_FLAGS "-g -O0 -pg -fprofile-arcs -ftest-coverage -std=c++11")
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

# Boost finding and / or unpacking
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
# End of boost wizardry

# Add other targets
add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(test)
add_executable(packer src/main.cpp)

target_link_libraries(packer packer_lib)
if ((NOT (DEFINED target-os)) OR (DEFINED target-os AND (NOT (target-os STREQUAL "windows"))))
    if (${DISPLAY})
	    target_link_libraries(packer ${SFML_LIBRARIES})
    endif ()
endif()

# Windows cross-compilation
if (DEFINED target-os AND target-os STREQUAL "windows")
    message(STATUS "cross compilation")
    target_link_libraries(${EXE_NAME} boost_program_options-mt-s)
    set(EXE_NAME_PLUGIN "${EXE_NAME}.exe")
# Normal compilation
else ()
	# Fome installing
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

# Copy runtime files to the build directory
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
