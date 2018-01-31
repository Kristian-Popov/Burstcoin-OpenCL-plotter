#include "plot_file_math.h"

#include "catch/single_include/catch.hpp"

static constexpr uint64_t nonceSizeInBytes_ = 256 * 1024; // 256 KiB
static constexpr uint64_t scoopSizeInBytes_ = 64;
static constexpr uint64_t scoopMaxNum_ = 4095;

TEST_CASE( "CalcStaggerStartOffsetInBytes works as expected in happy-path scenarios", "[PlotFileMath]" ) {
    REQUIRE( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 1, 1 ), 0 ) == 0 );
    REQUIRE( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 2, 1 ), 0 ) == 0 );
    REQUIRE( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 2, 1 ), 1 ) == nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 2, 2 ), 0 ) == 0 );
    REQUIRE( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 10, 5 ), 0 ) == 0 );
    REQUIRE( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 10, 5 ), 1 ) == 5 * nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 14, 7 ), 0 ) == 0 );
    REQUIRE( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 14, 7 ), 1 ) == 7 * nonceSizeInBytes_ );

    REQUIRE( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 4, 2 ), 0 ) == 0 );
    REQUIRE( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 4, 2 ), 1 ) == 2 * nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 12, 3 ), 0 ) == 0 );
    REQUIRE( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 12, 3 ), 1 ) == 3 * nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 12, 3 ), 2 ) == 6 * nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 12, 3 ), 3 ) == 9 * nonceSizeInBytes_ );
}

TEST_CASE( "CalcScoopRegionSizeInBytes works as expected in happy-path scenarios", "[PlotFileMath]" ) {
    REQUIRE( PlotFileMath::CalcScoopRegionSizeInBytes( PlotFileParams( 0, 0, 0, 0 ) ) == 0 );
    REQUIRE( PlotFileMath::CalcScoopRegionSizeInBytes( PlotFileParams( 0, 0, 1, 1 ) ) == scoopSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcScoopRegionSizeInBytes( PlotFileParams( 0, 0, 2, 1 ) ) == scoopSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcScoopRegionSizeInBytes( PlotFileParams( 0, 0, 2, 2 ) ) == 2 * scoopSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcScoopRegionSizeInBytes( PlotFileParams( 0, 0, 10, 5 ) ) == 5 * scoopSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcScoopRegionSizeInBytes( PlotFileParams( 0, 0, 14, 7 ) ) == 7 * scoopSizeInBytes_ );
}

TEST_CASE( "CalcScoopStartOffsetInBytes works as expected in happy-path scenarios", "[PlotFileMath]" ) {
    REQUIRE( PlotFileMath::CalcScoopStartOffsetInBytes( PlotFileParams( 0, 0, 1, 1 ), 0, 0 ) == 0 );
    REQUIRE( PlotFileMath::CalcScoopStartOffsetInBytes( PlotFileParams( 0, 0, 1, 1 ), 0, 1 ) == scoopSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcScoopStartOffsetInBytes( PlotFileParams( 0, 0, 1, 1 ), 0, 2 ) == 2 * scoopSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcScoopStartOffsetInBytes( PlotFileParams( 0, 0, 1, 1 ), 0, scoopMaxNum_ ) == scoopMaxNum_ * scoopSizeInBytes_ );
    
    REQUIRE( PlotFileMath::CalcScoopStartOffsetInBytes( PlotFileParams( 0, 0, 3, 1 ), 0, 0 ) == 0 );
    REQUIRE( PlotFileMath::CalcScoopStartOffsetInBytes( PlotFileParams( 0, 0, 3, 1 ), 0, 1 ) == scoopSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcScoopStartOffsetInBytes( PlotFileParams( 0, 0, 3, 1 ), 0, 2 ) == 2 * scoopSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcScoopStartOffsetInBytes( PlotFileParams( 0, 0, 3, 1 ), 0, scoopMaxNum_ ) == scoopMaxNum_ * scoopSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcScoopStartOffsetInBytes( PlotFileParams( 0, 0, 3, 1 ), 1, 0 ) == nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcScoopStartOffsetInBytes( PlotFileParams( 0, 0, 3, 1 ), 1, 1 ) == nonceSizeInBytes_ + scoopSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcScoopStartOffsetInBytes( PlotFileParams( 0, 0, 3, 1 ), 1, 2 ) == nonceSizeInBytes_ + 2 * scoopSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcScoopStartOffsetInBytes( PlotFileParams( 0, 0, 3, 1 ), 1, scoopMaxNum_ ) == nonceSizeInBytes_ + scoopMaxNum_ * scoopSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcScoopStartOffsetInBytes( PlotFileParams( 0, 0, 3, 1 ), 2, 0 ) == 2 * nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcScoopStartOffsetInBytes( PlotFileParams( 0, 0, 3, 1 ), 2, 1 ) == 2 * nonceSizeInBytes_ + scoopSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcScoopStartOffsetInBytes( PlotFileParams( 0, 0, 3, 1 ), 2, 2 ) == 2 * nonceSizeInBytes_ + 2 * scoopSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcScoopStartOffsetInBytes( PlotFileParams( 0, 0, 3, 1 ), 2, scoopMaxNum_ ) == 2 * nonceSizeInBytes_ + scoopMaxNum_ * scoopSizeInBytes_ );
}

TEST_CASE( "CalcStaggerCount works as expected in happy-path scenarios", "[PlotFileMath]" ) {
    REQUIRE( PlotFileMath::CalcStaggerCount( PlotFileParams( 0, 0, 1, 1 ) ) == 1 );
    REQUIRE( PlotFileMath::CalcStaggerCount( PlotFileParams( 0, 0, 2, 1 ) ) == 2 );
    REQUIRE( PlotFileMath::CalcStaggerCount( PlotFileParams( 0, 0, 3, 1 ) ) == 3 );

    REQUIRE( PlotFileMath::CalcStaggerCount( PlotFileParams( 0, 0, 2, 2 ) ) == 1 );
    REQUIRE( PlotFileMath::CalcStaggerCount( PlotFileParams( 0, 0, 4, 2 ) ) == 2 );
    REQUIRE( PlotFileMath::CalcStaggerCount( PlotFileParams( 0, 0, 1000, 10 ) ) == 100 );
}