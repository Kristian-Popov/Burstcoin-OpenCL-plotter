#pragma once

#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include "plot_file.h"

#include "plot_file_scanner.h"

/*
 * Searches all directories recursively and checks if any nonce is duplicated (i.e. space is wasted)
 */
class OverlapChecker
{
public:
    static void CheckForNonceOverlaps( const std::vector<std::string>& directories )
    {
        std::vector<std::unique_ptr<PlotFile>> plotFiles;
        for (const std::string& dir: directories)
        {
            Utils::AppendVectorToVector( plotFiles, PlotFileScanner::Scan( dir ) );
        }

        for ( const std::unique_ptr<PlotFile>& plotFile: plotFiles )
        {
            BOOST_LOG_TRIVIAL(debug) << "Found plot file at path " << plotFile->FileNameWithPath();

            // Verify that all pointers are not empty
            EXCEPTION_ASSERT( plotFile );
        }

        // TODO remove duplicate plot files (multiple representations of the same plot file)?
        std::vector<NonceNumRange> overlaps;
        ExecutePairWise( plotFiles.begin(), plotFiles.end(), [&overlaps]
        ( const std::unique_ptr<PlotFile>& lhs, const std::unique_ptr<PlotFile>& rhs )
        {
            boost::optional<NonceNumRange> possibleOverlap = lhs->CalcNonceRangeIntersectionWith( *rhs );
            if (possibleOverlap)
            {
                overlaps.push_back( possibleOverlap.value() );
            }
        } );

        if (overlaps.empty())
        {
            BOOST_LOG_TRIVIAL(info) << "No overlaps found";
        }
        else
        {
            // TODO remove duplicates in overlaps
            BOOST_LOG_TRIVIAL(error) << "Overlaps found: " << Utils::VectorToString( overlaps );
        }
    }
};
