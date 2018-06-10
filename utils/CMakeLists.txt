cmake_minimum_required (VERSION 3.1)

project(utils) # TODO is it needed? May be use one project for all executables/libraries?

# It is assumed that Boost can be found either by default location is OS (often a case on Linux) or
# it is located in <project source root>/contrib/boost
# Minimal Boost version is 1.58
SET(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.58 REQUIRED filesystem log program_options)

add_library( utils
	utils.cpp
	plot_file_math.cpp
	plot_file.cpp
	nonce_num_range.cpp
	nonce_num_set.cpp
	version.cpp

	plot_file.h
	plot_file_params.h
	utils.h
	plot_file_math.h
	optimizer.h
	nonce_num_range.h
	plot_file_scanner.h
	overlap_checker.h
	nonce_num_set.h
	plot_planner.h

	plotters/plotter_interface.h
	#plotters/mdcct_stub_plotter.h
	plotters/cryo_gpu_stub_plotter.h
)

target_include_directories (utils PUBLIC ${OpenCL_INCLUDE_DIRS} 
	${CMAKE_SOURCE_DIR}/contrib 
	${Boost_INCLUDE_DIRS} )

target_link_libraries (utils ${Boost_LIBRARIES})

set_property(TARGET utils PROPERTY CXX_STANDARD 14)
set_property(TARGET utils PROPERTY CXX_STANDARD_REQUIRED ON)
