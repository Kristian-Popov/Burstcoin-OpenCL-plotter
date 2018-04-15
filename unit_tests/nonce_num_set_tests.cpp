#include "catch/single_include/catch.hpp"

#include "nonce_num_set.h"

TEST_CASE( "Inverse works", "[NonceNumSet]" )
{
    {
        NonceNumSet set( {} );
        NonceNumSet inversedSet = set.Inverse();
        auto ranges = inversedSet.GetRanges();
        REQUIRE( ranges.size() == 1 );
        REQUIRE( *( ranges.begin() ) == NonceNumRange::ConstructUsingStartAndEnd( 0, ULLONG_MAX - 1 ) );
    }

    {
        NonceNumSet set( {
            NonceNumRange( 10, 10 )
        } );
        NonceNumSet inversedSet = set.Inverse();
        auto ranges = inversedSet.GetRanges();
        REQUIRE( ranges.size() == 2 );
        auto iter = ranges.cbegin();
        REQUIRE( *iter == NonceNumRange::ConstructUsingStartAndEnd( 0, 9 ) );
        ++iter;
        REQUIRE( *iter == NonceNumRange::ConstructUsingStartAndEnd( 20, ULLONG_MAX - 1 ) );
    }

    {
        NonceNumSet set( {
            NonceNumRange( 0, 10 )
        } );
        NonceNumSet inversedSet = set.Inverse();
        auto ranges = inversedSet.GetRanges();
        REQUIRE( ranges.size() == 1 );
        auto iter = ranges.cbegin();
        REQUIRE( *iter == NonceNumRange::ConstructUsingStartAndEnd( 10, ULLONG_MAX - 1 ) );
    }

    {
        NonceNumSet set( {
            NonceNumRange( ULLONG_MAX - 10, 10 )
        } );
        NonceNumSet inversedSet = set.Inverse();
        auto ranges = inversedSet.GetRanges();
        REQUIRE( ranges.size() == 1 );
        auto iter = ranges.cbegin();
        REQUIRE( *iter == NonceNumRange::ConstructUsingStartAndEnd( 0, ULLONG_MAX - 11 ) );
    }

    {
        NonceNumSet set( {
            NonceNumRange( 0, 10 ),
            NonceNumRange( ULLONG_MAX - 10, 10 ),
        } );
        NonceNumSet inversedSet = set.Inverse();
        auto ranges = inversedSet.GetRanges();
        REQUIRE( ranges.size() == 1 );
        REQUIRE( *( ranges.begin() ) == NonceNumRange::ConstructUsingStartAndEnd( 10, ULLONG_MAX - 11 ) );
    }

    {
        NonceNumSet set( {
            NonceNumRange( 10, 10 ),
            NonceNumRange( 1000, 100 ),
        } );
        NonceNumSet inversedSet = set.Inverse();
        auto ranges = inversedSet.GetRanges();
        REQUIRE( ranges.size() == 3 );
        auto iter = ranges.cbegin();
        REQUIRE( *iter == NonceNumRange::ConstructUsingStartAndEnd( 0, 9 ) );
        ++iter;
        REQUIRE( *iter == NonceNumRange::ConstructUsingStartAndEnd( 20, 999 ) );
        ++iter;
        REQUIRE( *iter == NonceNumRange::ConstructUsingStartAndEnd( 1100, ULLONG_MAX - 1 ) );
    }
}
