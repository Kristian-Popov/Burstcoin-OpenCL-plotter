#pragma once

#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include "plot_file.h"
#include "overlap_checker.h"
#include "nonce_num_set.h"
#include "plot_file_math.h"
#include "optimizer.h"

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
        const boost::filesystem::path& tempDirectory,
        bool forceDirectMode,
        uint64_t maxBytesToFill = ULLONG_MAX )
    {
        std::vector<std::string> allDirectories = directories; // Also include temporary directory to all checks and calculations
        allDirectories.push_back( tempDirectory.string() ); // Temporary storage must be in the end

        int overlapCount = OverlapChecker::CheckForNonceOverlaps( allDirectories );
        if ( overlapCount > 0 )
        {
            BOOST_LOG_TRIVIAL(error) << "Found overlaps, please remove them and then run planner";
            return;
        }

        // TODO there's a very similar thing at a beginning of OverlapChecker::CheckForOverlaps()
        std::vector<std::shared_ptr<PlotFile>> plotFiles;
        for (const std::string& dir: allDirectories)
        {
            Utils::AppendVectorToVector( plotFiles, PlotFileScanner::Scan( dir ) );
        }

        std::sort( plotFiles.begin(), plotFiles.end() );
        plotFiles.erase( std::unique( plotFiles.begin(), plotFiles.end() ), plotFiles.end() );

        std::vector<NonceNumRange> ranges;
        std::vector<std::shared_ptr<PlotFile>> plotFilesInTempStorage;
        for ( const std::shared_ptr<PlotFile>& plotFile: plotFiles )
        {
            // Verify that all pointers are not empty
            EXCEPTION_ASSERT( plotFile );
            ranges.push_back( plotFile->Params().nonceNumRange_ ); // Add all ranges

            if ( !forceDirectMode && plotFile->DoesItBelongToDirectory( tempDirectory ) )
            {
                plotFilesInTempStorage.push_back( plotFile );
            }
        }

        // Create a nonce number set and inverse it
        NonceNumSet nonceNumSet = NonceNumSet( ranges ).Inverse();

        // Optimize all files in temporary storage
        for ( const std::shared_ptr<PlotFile>& plotFile: plotFilesInTempStorage )
        {
            uint64_t plotFileSizeInByes = PlotFileMath::CalcPlotFileSize( plotFile->Params() );
            auto dirWithEnoughSpace = std::find_if( directories.cbegin(), directories.cend(),
                [plotFileSizeInByes] ( const std::string& dir )
                {
                    return CalcFreeSpaceInDirInBytes( dir ) >= plotFileSizeInByes;
                } );
            if( dirWithEnoughSpace == directories.cend() )
            {
                BOOST_LOG_TRIVIAL(error) << "Found a plot file in temporary directory, but there's no space where to move it. "
                    "You make more efficient usage of your space, remove all files from temporary storage (including plot files) "
                    "and start again, this utility should pick plot file sizes in a more efficient way";
                throw std::runtime_error( "Moving or optimizing plot file from temporary storage failed since there's no space fo that" );
            }

            if( plotFile->IsFullyOptimized() )
            {
                boost::filesystem::path newFilePath = *dirWithEnoughSpace;
                newFilePath /= PlotFile::BuildPlotFileNameForParams( plotFile->Params() );
                BOOST_LOG_TRIVIAL( info ) << "Found file " << plotFile->FileNameWithPath() << " in temporary storage "
                    " that is fully optimized, moving it to " << newFilePath.string();
                boost::filesystem::rename( plotFile->FileNameWithPath(), newFilePath );
            }
            else
            {
                BOOST_LOG_TRIVIAL( info ) << "Found file " << plotFile->FileNameWithPath() << " in temporary storage "
                    " that is not optimized, optimizing it and putting to directory " << *dirWithEnoughSpace;

                Optimizer::Optimize( plotFile, *dirWithEnoughSpace );

                BOOST_LOG_TRIVIAL( info ) << "Removing original file " << plotFile->FileNameWithPath();
                EXCEPTION_ASSERT( boost::filesystem::remove( plotFile->FileNameWithPath() ) );
            }
        }

        // Start filling the space
        uint64_t maxNoncesToFill = PlotFileMath::CalcNonceCountInBytes( maxBytesToFill );
        for ( const std::string& dir: allDirectories )
        {
            BOOST_LOG_TRIVIAL(debug) << "Preparing to fill a directory " << dir;

            uint64_t freeSpaceInBytes = CalcFreeSpaceInDirInBytes( dir );
            uint64_t spaceAvailableForThisManyNonces = PlotFileMath::CalcNonceCountInBytes( freeSpaceInBytes );
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
                PlottingMode mode = forceDirectMode ? PlottingMode::Direct : PlottingMode::Buffer;
                if ( nonceNotFittingInStagger > 0 )
                {
                    // If nonce count is smaller than stagger, just use it. Plus we can use direct mode
                    if ( noncesToFillOnThisIteration < maxStaggerSizeInNonces )
                    {
                        staggerSizeInNonces = noncesToFillOnThisIteration;
                        mode = PlottingMode::Direct;
                        // TODO plot directly to destination folder
                    }
                    else
                    {
                        // Decrease range size if it doesn't fit stagger perfectly
                        noncesToFillOnThisIteration -= nonceNotFittingInStagger;
                        // TODO plot to temporary storage
                    }
                }

                NonceNumRange range = nonceNumSet.CutPieceAtBeginning( noncesToFillOnThisIteration );
                EXCEPTION_ASSERT( range.SizeInNonce() <= noncesToFillOnThisIteration );

                PlotFileParams params( accountNumericId, range, staggerSizeInNonces );
                NonceNumRange newRange = plotter->Plot( dir, params, mode );
                noncesToFill -= newRange.SizeInNonce();
                EXCEPTION_ASSERT( range == newRange );
                // Currently new range returned by Plot() must be the same as starting one

                // TODO if plottig was done to temporary storage and we're not filling temp storage, optimize it
            }
        }
    }

    static boost::uintmax_t CalcFreeSpaceInDirInBytes( const boost::filesystem::path& dirPath )
    {
        boost::filesystem::space_info spaceInfo = boost::filesystem::space( dirPath );
        EXCEPTION_ASSERT( spaceInfo.available != -1 );
        return spaceInfo.available;
    }
};
