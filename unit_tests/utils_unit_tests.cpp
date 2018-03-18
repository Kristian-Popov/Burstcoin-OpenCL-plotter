#include "catch/single_include/catch.hpp"

#include "utils.h"

// Ensure that ExecutePairWise doesn't call a function when range is empty
TEST_CASE( "ExecutePairWise works on empty range", "[Utils]" )
{
    std::function<void(int, int)> func = [] ( int lhs, int rhs )
        {
            FAIL();
        };
    std::vector<int> data;
    ExecutePairWise( data.begin(), data.end(), func );
}

// Ensure that ExecutePairWise doesn't call a function when range is only one element long
TEST_CASE( "ExecutePairWise works on single element range", "[Utils]" )
{
    std::function<void(int, int)> func = [] ( int lhs, int rhs )
        {
            FAIL();
        };
    std::vector<int> data { 42 };
    ExecutePairWise( data.begin(), data.end(), func );
}

// Ensure that ExecutePairWise calls a function exactly once on a two element long
TEST_CASE( "ExecutePairWise works on two element range", "[Utils]" )
{
    std::vector<std::pair<int, int>> executedOn;
    std::function<void(int, int)> func = [&executedOn] ( int lhs, int rhs )
        {
            executedOn.push_back( std::make_pair( lhs, rhs ) );
        };
    std::vector<int> data { 10, 42 };
    ExecutePairWise( data.begin(), data.end(), func );
    REQUIRE( executedOn.size() == 1 );
    REQUIRE( executedOn.at( 0 ) == std::make_pair( 10, 42 ) );
}
