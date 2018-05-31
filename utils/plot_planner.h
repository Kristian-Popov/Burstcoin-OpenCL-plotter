#pragma once

#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include "plot_file.h"
#include "overlap_checker.h"
#include "nonce_num_set.h"
#include "plot_file_math.h"

#include "plotters/plotter_interface.h"

#include "plot_file_scanner.h"

/*
 * Collects information about all existing plot files, calculates free nonces and schedules a plotter to generate
 * plot files to fill the space
 */
class PlotPlanner
{
public:
    static void FillSpace( const std::shared_ptr<PlotterInterface>& plotter,
        uint64_t accountNumericId,
        const std::vector<std::string>& directories,
        uint64_t maxStaggerSizeInNonces,
        bool forceDirectMode,
        uint64_t maxBytesToFill = ULLONG_MAX )
    {
        EXCEPTION_ASSERT( forceDirectMode ); // Buffer mode is not supported yet

        int overlapCount = OverlapChecker::CheckForNonceOverlaps( directories );
        if ( overlapCount > 0 )
        {
            BOOST_LOG_TRIVIAL(error) << "Found overlaps, please remove them and then run planner";
            return;
        }

        // TODO there's a very similar thing at a beginning of OverlapChecker::CheckForOverlaps()
        std::vector<std::shared_ptr<PlotFile>> plotFiles;
        for (const std::string& dir: directories)
        {
            Utils::AppendVectorToVector( plotFiles, PlotFileScanner::Scan( dir ) );
        }

        std::sort( plotFiles.begin(), plotFiles.end() );
        plotFiles.erase( std::unique( plotFiles.begin(), plotFiles.end() ), plotFiles.end() );

        std::vector<NonceNumRange> ranges;
        for ( const std::shared_ptr<PlotFile>& plotFile: plotFiles )
        {
            // Verify that all pointers are not empty
            EXCEPTION_ASSERT( plotFile );
            ranges.push_back( plotFile->Params().nonceNumRange_ );
        }

        // Create a nonce number set and inverse it
        NonceNumSet nonceNumSet = NonceNumSet( ranges ).Inverse();

        // Start filling the space
        uint64_t maxNoncesToFill = PlotFileMath::CalcNonceCountInBytes( maxBytesToFill );
        for ( const std::string& dir: directories )
        {
            BOOST_LOG_TRIVIAL(debug) << "Preparing to fill a directory " << dir;

            boost::filesystem::path dirPath = dir;
            boost::filesystem::space_info spaceInfo = boost::filesystem::space( dirPath );
            EXCEPTION_ASSERT( spaceInfo.available != -1 );
            uint64_t spaceAvailableForThisManyNonces = PlotFileMath::CalcNonceCountInBytes( spaceInfo.available );
            uint64_t noncesToFill = std::min( spaceAvailableForThisManyNonces, maxNoncesToFill );
            EXCEPTION_ASSERT( maxNoncesToFill >= noncesToFill );
            maxNoncesToFill -= noncesToFill;

            BOOST_LOG_TRIVIAL(debug) << "I'm still allowed to fill " << maxNoncesToFill << " nonces";
            BOOST_LOG_TRIVIAL(debug) << "Current directory has space for " << spaceAvailableForThisManyNonces << " nonces";
            BOOST_LOG_TRIVIAL(debug) << "Will generate " << noncesToFill << " nonces for current directory";

            while( noncesToFill > 0 )
            {
                uint64_t noncesToFillOnThisIteration = noncesToFill;
                uint64_t nonceNotFittingInStagger = noncesToFillOnThisIteration % maxStaggerSizeInNonces;
                uint64_t staggerSizeInNonces = maxStaggerSizeInNonces;
                PlottingMode mode;
                if ( nonceNotFittingInStagger > 0 )
                {
                    // If nonce count is smaller than stagger, just use it. Plus we can use direct mode
                    if ( noncesToFillOnThisIteration < maxStaggerSizeInNonces )
                    {
                        staggerSizeInNonces = noncesToFillOnThisIteration;
                        mode = PlottingMode::Direct;
                    }
                    else
                    {
                        // Decrease range size if it doesn't fit stagger perfectly
                        noncesToFillOnThisIteration -= nonceNotFittingInStagger;
                        mode = forceDirectMode ? PlottingMode::Direct : PlottingMode::Buffer;
                    }
                }

                NonceNumRange range = nonceNumSet.CutPieceAtBeginning( noncesToFillOnThisIteration );
                EXCEPTION_ASSERT( range.SizeInNonce() <= noncesToFillOnThisIteration );

                PlotFileParams params( accountNumericId, range, staggerSizeInNonces );
                NonceNumRange newRange = plotter->Plot( dir, params, mode );
                noncesToFill -= newRange.SizeInNonce();
                EXCEPTION_ASSERT( range == newRange );
                // Currently new range returned by Plot() must be the same as starting one
            }
        }

    }
};
