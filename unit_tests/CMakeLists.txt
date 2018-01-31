cmake_minimum_required (VERSION 3.1)

project(unit_tests)

add_executable (unit_tests 
	unit_tests.cpp
	plot_file_tests.cpp
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
