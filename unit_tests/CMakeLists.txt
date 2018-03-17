cmake_minimum_required (VERSION 3.1)

# Set used language
enable_language(CXX)

# It is assumed that Boost can be found either by default location is OS (often a case on Linux) or
# it is located in <project source root>/contrib/boost
# Minimal Boost version is 1.58
SET(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.58 REQUIRED filesystem log program_options)

# Look for libraries in the following folders
link_directories(${Boost_LIBRARY_DIRS})

project(unit_tests)

add_executable (unit_tests 
	unit_tests.cpp
	plot_file_tests.cpp
	plot_file_math_tests.cpp
	nonce_num_range_tests.cpp
)

target_include_directories (unit_tests PUBLIC ${OpenCL_INCLUDE_DIRS} 
	${CMAKE_SOURCE_DIR}/contrib 
	${Boost_INCLUDE_DIRS} 
	${CMAKE_SOURCE_DIR}/utils )
target_link_libraries (unit_tests ${OpenCL_LIBRARY} ${Boost_LIBRARIES} utils)

if (UNIX)
    target_link_libraries (unit_tests pthread)
endif (UNIX)

set_property(TARGET unit_tests PROPERTY CXX_STANDARD 14)
set_property(TARGET unit_tests PROPERTY CXX_STANDARD_REQUIRED ON)
