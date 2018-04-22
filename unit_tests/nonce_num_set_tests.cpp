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

TEST_CASE( "CutPieceAtBeginning() works", "[NonceNumSet]" )
{
    {
        NonceNumSet set( {
            NonceNumRange( 0, 10 )
        } );
        REQUIRE( set.CutPieceAtBeginning( 10 ) == NonceNumRange( 0, 10 ) );
        REQUIRE( set.GetRanges().empty() );
    }
    {
        NonceNumSet set( {
            NonceNumRange( 0, 10 )
        } );
        REQUIRE( set.CutPieceAtBeginning( 20 ) == NonceNumRange( 0, 10 ) );
        REQUIRE( set.GetRanges().empty() );
    }
    {
        NonceNumSet set( {
            NonceNumRange( 0, 10 )
        } );
        REQUIRE( set.CutPieceAtBeginning( 5 ) == NonceNumRange( 0, 5 ) );
        REQUIRE( set.CutPieceAtBeginning( 5 ) == NonceNumRange( 5, 5 ) );
        REQUIRE( set.GetRanges().empty() );
    }
    {
        NonceNumSet set( {
            NonceNumRange( 10, 100 )
        } );
        REQUIRE( set.CutPieceAtBeginning( 5 ) == NonceNumRange( 10, 5 ) );
        REQUIRE( set.CutPieceAtBeginning( 50 ) == NonceNumRange( 15, 50 ) );
        REQUIRE( set.CutPieceAtBeginning( 100 ) == NonceNumRange( 65, 45 ) );
        REQUIRE( set.GetRanges().empty() );
    }
}
