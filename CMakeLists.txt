cmake_minimum_required (VERSION 3.1)

# Set used language
enable_language(CXX)

# Find OpenCL and Boost libraries.
# It is assumed that Boost can be found either by default location is OS (often a case on Linux) or
# it is located in <project source root>/contrib/boost
# Minimal Boost version is 1.65.1
find_package(OpenCL REQUIRED)
find_package(Boost 1.65.1 REQUIRED )

# Look for libraries in the following folders
link_directories(${OpenCL_LIBRARY} ${Boost_LIBRARY_DIRS})

# Add subprojects
add_subdirectory(utils)
add_subdirectory(unit_tests)

# Add main project
project(BurstcoinOpenCLPlotter)

add_executable (burstcoin-opencl-plotter 
	main.cpp 
)

target_include_directories (burstcoin-opencl-plotter PUBLIC ${CMAKE_SOURCE_DIR}
	${OpenCL_INCLUDE_DIRS} 
	${CMAKE_SOURCE_DIR}/contrib 
	${Boost_INCLUDE_DIRS} 
	${CMAKE_SOURCE_DIR}/utils )
target_link_libraries (burstcoin-opencl-plotter ${OpenCL_LIBRARY} ${Boost_LIBRARIES} utils)

if (UNIX)
    target_link_libraries (main pthread)
endif (UNIX)

set_property(TARGET burstcoin-opencl-plotter PROPERTY CXX_STANDARD 14)
set_property(TARGET burstcoin-opencl-plotter PROPERTY CXX_STANDARD_REQUIRED ON)
