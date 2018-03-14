cmake_minimum_required (VERSION 3.1)

project(utils)

# It is assumed that Boost can be found either by default location is OS (often a case on Linux) or
# it is located in <project source root>/contrib/boost
# Minimal Boost version is 1.58
SET(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.58 REQUIRED filesystem log program_options)

add_library( utils
	utils.cpp
	plot_file_math.cpp
	plot_file.cpp

	plot_file.h
	plot_file_params.h
	utils.h
	plot_file_math.h
	optimizer.h
)

target_include_directories (utils PUBLIC ${OpenCL_INCLUDE_DIRS} 
	${CMAKE_SOURCE_DIR}/contrib 
	${Boost_INCLUDE_DIRS} )

target_link_libraries (utils ${Boost_LIBRARIES})

set_property(TARGET utils PROPERTY CXX_STANDARD 14)
set_property(TARGET utils PROPERTY CXX_STANDARD_REQUIRED ON)
