#include "plot_file_math.h"

#include "catch/single_include/catch.hpp"

static constexpr uint64_t nonceSizeInBytes_ = 256 * 1024; // 256 KiB
static constexpr uint64_t scoopSizeInBytes_ = 64;
static constexpr uint64_t scoopMaxNum_ = 4095;

// Tests for CalcStaggerStartOffsetInBytes
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

TEST_CASE( "CalcStaggerStartOffsetInBytes throws exception when trying to access invalid stagger", "[PlotFileMath]" ) {
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 1, 1 ), 1 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 2, 1 ), 2 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 2, 1 ), 3 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 2, 1 ), 4 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 2, 1 ), 1000 ) );
    
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 2, 2 ), 1 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 2, 2 ), 2 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 2, 2 ), 3 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 2, 2 ), 4 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 2, 2 ), 5 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 2, 2 ), 1000 ) );
    
    
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 10, 5 ), 2 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 10, 5 ), 3 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 10, 5 ), 4 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 10, 5 ), 5 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 10, 5 ), 1000 ) );
    
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 14, 7 ), 2 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 14, 7 ), 3 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 14, 7 ), 4 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 14, 7 ), 5 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 14, 7 ), 1000 ) );

    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 12, 3 ), 4 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 12, 3 ), 5 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 12, 3 ), 6 ) );
    REQUIRE_THROWS( PlotFileMath::CalcStaggerStartOffsetInBytes( PlotFileParams( 0, 0, 12, 3 ), 1000 ) );
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

TEST_CASE( "CalcPlotFileSize works as expected in happy-path scenarios", "[PlotFileMath]" ) {
    REQUIRE( PlotFileMath::CalcPlotFileSize( PlotFileParams( 0, 0, 1, 1 ) ) == nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcPlotFileSize( PlotFileParams( 0, 0, 2, 1 ) ) == 2 * nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcPlotFileSize( PlotFileParams( 0, 0, 3, 1 ) ) == 3 * nonceSizeInBytes_ );

    REQUIRE( PlotFileMath::CalcPlotFileSize( PlotFileParams( 0, 0, 2, 2 ) ) == 2 * nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcPlotFileSize( PlotFileParams( 0, 0, 4, 2 ) ) == 4 * nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcPlotFileSize( PlotFileParams( 0, 0, 4, 4 ) ) == 4 * nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcPlotFileSize( PlotFileParams( 0, 0, 1000, 10 ) ) == 1000 * nonceSizeInBytes_ );
}

TEST_CASE( "CalcStaggerSizeInBytes works as expected in happy-path scenarios", "[PlotFileMath]" ) {
    REQUIRE( PlotFileMath::CalcStaggerSizeInBytes( PlotFileParams( 0, 0, 1, 1 ) ) == nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcStaggerSizeInBytes( PlotFileParams( 0, 0, 2, 1 ) ) == 1 * nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcStaggerSizeInBytes( PlotFileParams( 0, 0, 2, 2 ) ) == 2 * nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcStaggerSizeInBytes( PlotFileParams( 0, 0, 3, 1 ) ) == 1 * nonceSizeInBytes_ );

    REQUIRE( PlotFileMath::CalcStaggerSizeInBytes( PlotFileParams( 0, 0, 2, 2 ) ) == 2 * nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcStaggerSizeInBytes( PlotFileParams( 0, 0, 4, 2 ) ) == 2 * nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcStaggerSizeInBytes( PlotFileParams( 0, 0, 4, 4 ) ) == 4 * nonceSizeInBytes_ );
    REQUIRE( PlotFileMath::CalcStaggerSizeInBytes( PlotFileParams( 0, 0, 1000, 10 ) ) == 10 * nonceSizeInBytes_ );
}