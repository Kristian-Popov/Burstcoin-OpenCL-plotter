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

# Add subprojects
#add_subdirectory(utils)
#add_subdirectory(unit_tests)

# Add main project
project(BurstcoinOpenCLPlotter)

add_executable (burstcoin-optimizer
    optimizer-main.cpp
)

target_include_directories (burstcoin-optimizer PUBLIC ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/contrib
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/utils )
target_link_libraries (burstcoin-optimizer ${Boost_LIBRARIES} utils)

set_property(TARGET burstcoin-optimizer PROPERTY CXX_STANDARD 14)
set_property(TARGET burstcoin-optimizer PROPERTY CXX_STANDARD_REQUIRED ON)
