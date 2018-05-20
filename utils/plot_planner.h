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
        uint64_t maxBytesToFill = ULLONG_MAX )
    {
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

            BOOST_LOG_TRIVIAL(debug) << "Will generate " << noncesToFill << " nonces for current directory";

            while( noncesToFill > 0 )
            {
                // TODO if we get our range shrinked due to fitting to stagger, we get holes because
                // "range is already removed from "nonceNumSet", hole appears - [new range end + 1, original range end]
                NonceNumRange range = nonceNumSet.CutPieceAtBeginning( noncesToFill );
                EXCEPTION_ASSERT( range.SizeInNonce() <= noncesToFill );

                NonceNumRange newRange = plotter->Plot( dir, accountNumericId, range );
                noncesToFill -= newRange.SizeInNonce();
            }
        }

    }
};
