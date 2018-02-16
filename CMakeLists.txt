cmake_minimum_required (VERSION 3.1)

# Set used language
enable_language(CXX)

# It is assumed that Boost can be found either by default location is OS (often a case on Linux) or
# it is located in <project source root>/contrib/boost
# Minimal Boost version is 1.65.1
find_package(Boost 1.65.1 REQUIRED )

# Look for libraries in the following folders
link_directories(${Boost_LIBRARY_DIRS})

# Add subprojects
add_subdirectory(utils)
add_subdirectory(unit_tests)

# Add main project
project(BurstcoinOpenCLPlotter)

add_executable (burstcoin-optimizer
    optimizer-main.cpp
)

target_include_directories (burstcoin-optimizer PUBLIC ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/contrib
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/utils )
target_link_libraries (burstcoin-optimizer ${OpenCL_LIBRARY} ${Boost_LIBRARIES} utils)

if (UNIX)
    target_link_libraries (burstcoin-optimizer pthread)
endif (UNIX)

set_property(TARGET burstcoin-optimizer PROPERTY CXX_STANDARD 14)
set_property(TARGET burstcoin-optimizer PROPERTY CXX_STANDARD_REQUIRED ON)
