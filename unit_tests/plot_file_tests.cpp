#include "catch/single_include/catch.hpp"

#include "plot_file.h"

// TODO move PlotFileMath tests to plot_file_math_tests.cpp
TEST_CASE( "IsNameValid should work correctly", "[PlotFile]" )
{
    REQUIRE( PlotFile::IsNameValid( boost::filesystem::path( "0_0_0_0" ) ) == true );
    REQUIRE( PlotFile::IsNameValid( boost::filesystem::path( "4444_3333_1111_2222" ) ) == true );
    REQUIRE( PlotFile::IsNameValid( boost::filesystem::path( "9_9_9_9" ) ) == true );
    REQUIRE( PlotFile::IsNameValid( boost::filesystem::path( "9_9_9_" ) ) == false );
    REQUIRE( PlotFile::IsNameValid( boost::filesystem::path( "9_9__9" ) ) == false );
    REQUIRE( PlotFile::IsNameValid( boost::filesystem::path( "9__9_9" ) ) == false );
    REQUIRE( PlotFile::IsNameValid( boost::filesystem::path( "_9_9_9" ) ) == false );
    REQUIRE( PlotFile::IsNameValid( boost::filesystem::path( "9_9_9_9." ) ) == false );
    REQUIRE( PlotFile::IsNameValid( boost::filesystem::path( "9_9_9_9.a" ) ) == true );
    REQUIRE( PlotFile::IsNameValid( boost::filesystem::path( "9_9_9_9.abc" ) ) == true );
    REQUIRE( PlotFile::IsNameValid( boost::filesystem::path( "45459_34349_93434_9234.abc" ) ) == true );
    REQUIRE( PlotFile::IsNameValid( boost::filesystem::path( "lol" ) ) == false );
    REQUIRE( PlotFile::IsNameValid( boost::filesystem::path( "5_" ) ) == false );
    REQUIRE( PlotFile::IsNameValid( boost::filesystem::path( "5_.a" ) ) == false );
}