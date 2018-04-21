#include "catch/single_include/catch.hpp"

#include "nonce_num_range.h"

TEST_CASE( "Basic methods of NonceNumRange works as expected in happy-path scenarios", "[NonceNumRange]" )
{
    {
        NonceNumRange range1( 0, 10 );
        REQUIRE( range1.StartNonceNum() == 0 );
        REQUIRE( range1.SizeInNonce() == 10 );
    }

    {
        NonceNumRange range2( 1000, 1 );
        REQUIRE( range2.StartNonceNum() == 1000 );
        REQUIRE( range2.SizeInNonce() == 1 );
    }

    {
        NonceNumRange range3( 1, 2 );
        REQUIRE( range3.StartNonceNum() == 1 );
        REQUIRE( range3.SizeInNonce() == 2 );
    }

    {
        NonceNumRange range4( 1000000, 9 );
        REQUIRE( range4.StartNonceNum() == 1000000 );
        REQUIRE( range4.SizeInNonce() == 9 );
    }
}

TEST_CASE( "LastNonceNum method works as expected", "[NonceNumRange]" )
{
    {
        NonceNumRange range1( 0, 10 );
        REQUIRE( range1.LastNonceNum() == 9 );
    }

    {
        NonceNumRange range2( 1000, 1 );
        REQUIRE( range2.LastNonceNum() == 1000 );
    }

    {
        NonceNumRange range3( 1, 2 );
        REQUIRE( range3.LastNonceNum() == 2 );
    }

    {
        NonceNumRange range4( 1000000, 9 );
        REQUIRE( range4.LastNonceNum() == 1000008 );
    }

    {
        NonceNumRange range5( ULLONG_MAX - 1, 2 );
        REQUIRE( range5.LastNonceNum() == ULLONG_MAX );
    }

    {
        NonceNumRange range( ULLONG_MAX, 1 );
        REQUIRE( range.LastNonceNum() == ULLONG_MAX );
    }
}

TEST_CASE( "CalcIntersectionWith method works as expected", "[NonceNumRange]" )
{
    {
        NonceNumRange lhs( 0, 10 );
        NonceNumRange rhs( 10, 10 );
        REQUIRE( !lhs.CalcIntersectionWith( rhs ) );
    }

    {
        NonceNumRange lhs( 0, 5 );
        NonceNumRange rhs( 5, 5 );
        REQUIRE( !lhs.CalcIntersectionWith( rhs ) );
    }

    {
        NonceNumRange lhs( 1000, 10 );
        NonceNumRange rhs( 10, 10 );
        REQUIRE( !lhs.CalcIntersectionWith( rhs ) );
    }

    {
        NonceNumRange lhs( 0, 10 );
        NonceNumRange rhs( 0, 10 );
        REQUIRE( lhs.CalcIntersectionWith( rhs ) == rhs );
    }

    {
        NonceNumRange lhs( 0, 10 );
        NonceNumRange rhs( 0, 11 );
        REQUIRE( lhs.CalcIntersectionWith( rhs ) == lhs );
    }

    {
        NonceNumRange lhs( 100, 100 );
        NonceNumRange rhs( 100, 100 );
        REQUIRE( lhs.CalcIntersectionWith( rhs ) == lhs );
    }

    {
        NonceNumRange lhs( 10, 10 );
        NonceNumRange rhs( 0, 10 );
        REQUIRE( !lhs.CalcIntersectionWith( rhs ) );
    }

    {
        NonceNumRange lhs( 0, 20 );
        NonceNumRange rhs( 15, 10 );
        REQUIRE( lhs.CalcIntersectionWith( rhs ) == NonceNumRange( 15, 5 ) );
    }

    {
        NonceNumRange lhs( 0, 10 );
        NonceNumRange rhs( 1, 1 );
        REQUIRE( lhs.CalcIntersectionWith( rhs ) == rhs );
    }

    {
        NonceNumRange lhs( 1000, 1000 );
        NonceNumRange rhs( 1000, 20000 );
        REQUIRE( lhs.CalcIntersectionWith( rhs ) == lhs );
    }
}

TEST_CASE( "Comparison of NonceNumRange by operator< works as expected", "[NonceNumRange]" )
{
    {
        NonceNumRange lhs( 1000, 1000 );
        NonceNumRange rhs( 1000, 20000 );
        REQUIRE( lhs < rhs );
    }
    {
        NonceNumRange lhs( 2000, 1000 );
        NonceNumRange rhs( 1000, 20000 );
        REQUIRE( !( lhs < rhs ) );
    }
    {
        NonceNumRange lhs( 2000, 1000 );
        NonceNumRange rhs( 1000, 1000 );
        REQUIRE( !( lhs < rhs ) );
    }
    {
        NonceNumRange lhs( 2000, 2000 );
        NonceNumRange rhs( 2000, 1000 );
        REQUIRE( !( lhs < rhs ) );
    }
    {
        NonceNumRange lhs( 1000, 1000 );
        NonceNumRange rhs( 1000, 1000 );
        REQUIRE( !( lhs < rhs ) );
    }
    {
        NonceNumRange lhs( 0, 1000 );
        NonceNumRange rhs( 0, 1000 );
        REQUIRE( !( lhs < rhs ) );
    }
}

TEST_CASE( "NonceNumRange throws an exception in constructor if nonce number overflow will happen", "[NonceNumRange]" )
{
    REQUIRE_THROWS( NonceNumRange( ULLONG_MAX, 10 ) );
    REQUIRE_THROWS( NonceNumRange( ULLONG_MAX - 10, 20 ) );
    REQUIRE_THROWS( NonceNumRange( ULLONG_MAX, 2 ) );
    REQUIRE_THROWS( NonceNumRange( ULLONG_MAX - 1, 3 ) );
    REQUIRE_THROWS( NonceNumRange( ULLONG_MAX - 2, 4 ) );
}

TEST_CASE( "Split() works as expected", "[NonceNumRange]" )
{
    {
        auto splitResult = NonceNumRange( 0, 10 ).Split( 10 );
        REQUIRE( splitResult.first == NonceNumRange( 0, 10 ) );
        REQUIRE( !splitResult.second );
    }
    {
        auto splitResult = NonceNumRange( 0, 1000 ).Split( 1000 );
        REQUIRE( splitResult.first == NonceNumRange( 0, 1000 ) );
        REQUIRE( !splitResult.second );
    }
    {
        auto splitResult = NonceNumRange( 100, 100 ).Split( 100 );
        REQUIRE( splitResult.first == NonceNumRange( 100, 100 ) );
        REQUIRE( !splitResult.second );
    }
    {
        auto splitResult = NonceNumRange( 100, 100 ).Split( 200 );
        REQUIRE( splitResult.first == NonceNumRange( 100, 100 ) );
        REQUIRE( !splitResult.second );
    }
    {
        auto splitResult = NonceNumRange( 0, 1000 ).Split( 2000 );
        REQUIRE( splitResult.first == NonceNumRange( 0, 1000 ) );
        REQUIRE( !splitResult.second );
    }
    {
        auto splitResult = NonceNumRange( 0, 100 ).Split( 50 );
        REQUIRE( splitResult.first == NonceNumRange( 0, 50 ) );
        REQUIRE( splitResult.second == NonceNumRange( 50, 50 ) );
    }
    {
        auto splitResult = NonceNumRange( 0, 100 ).Split( 1 );
        REQUIRE( splitResult.first == NonceNumRange( 0, 1 ) );
        REQUIRE( splitResult.second == NonceNumRange( 1, 99 ) );
    }
}

TEST_CASE( "Few sequential calls to Split() works as expected", "[NonceNumRange]" )
{
    {
        auto splitResult1 = NonceNumRange( 0, 100 ).Split( 1 );
        REQUIRE( splitResult1.first == NonceNumRange( 0, 1 ) );
        REQUIRE( splitResult1.second == NonceNumRange( 1, 99 ) );

        {
            auto splitResult2 = splitResult1.second->Split( 1 );
            REQUIRE( splitResult2.first == NonceNumRange( 1, 1 ) );
            REQUIRE( splitResult2.second == NonceNumRange( 2, 98 ) );
        }
    }
}
