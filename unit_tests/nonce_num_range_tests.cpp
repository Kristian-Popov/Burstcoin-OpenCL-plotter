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

TEST_CASE( "OverlapsWith method works as expected", "[NonceNumRange]" )
{
    {
        NonceNumRange lhs( 0, 10 );
        NonceNumRange rhs( 10, 10 );
        REQUIRE( lhs.OverlapsWith( rhs ) == false );
    }

    {
        NonceNumRange lhs( 0, 5 );
        NonceNumRange rhs( 5, 5 );
        REQUIRE( lhs.OverlapsWith( rhs ) == false );
    }

    {
        NonceNumRange lhs( 1000, 10 );
        NonceNumRange rhs( 10, 10 );
        REQUIRE( lhs.OverlapsWith( rhs ) == false );
    }

    {
        NonceNumRange lhs( 0, 10 );
        NonceNumRange rhs( 0, 10 );
        REQUIRE( lhs.OverlapsWith( rhs ) == true );
    }

    {
        NonceNumRange lhs( 0, 10 );
        NonceNumRange rhs( 0, 11 );
        REQUIRE( lhs.OverlapsWith( rhs ) == true );
    }

    {
        NonceNumRange lhs( 100, 100 );
        NonceNumRange rhs( 100, 100 );
        REQUIRE( lhs.OverlapsWith( rhs ) == true );
    }

    {
        NonceNumRange lhs( 10, 10 );
        NonceNumRange rhs( 0, 10 );
        REQUIRE( lhs.OverlapsWith( rhs ) == false );
    }

    {
        NonceNumRange lhs( 0, 20 );
        NonceNumRange rhs( 10, 10 );
        REQUIRE( lhs.OverlapsWith( rhs ) == true );
    }

    {
        NonceNumRange lhs( 0, 10 );
        NonceNumRange rhs( 1, 1 );
        REQUIRE( lhs.OverlapsWith( rhs ) == true );
    }

    {
        NonceNumRange lhs( 1000, 1000 );
        NonceNumRange rhs( 1000, 20000 );
        REQUIRE( lhs.OverlapsWith( rhs ) == true );
    }
}

#if 0
// TODO enable this test when robust code for overflow check is implemented
TEST_CASE( "NonceNumRange throws an exception in constructor if nonce number overflow will happen", "[NonceNumRange]" )
{
    REQUIRE_THROWS( NonceNumRange( ULLONG_MAX, 10 ) );
    REQUIRE_THROWS( NonceNumRange( ULLONG_MAX - 10, 20 ) );
    REQUIRE_THROWS( NonceNumRange( ULLONG_MAX - 1, 2 ) );
    REQUIRE_THROWS( NonceNumRange( ULLONG_MAX - 1000000, 1000001 ) );
}
#endif
