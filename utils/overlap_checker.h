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
        std::vector<PlotFile> plotFiles;
        for (const std::string& dir: directories)
        {
            Utils::AppendVectorToVector( plotFiles, PlotFileScanner::Scan( dir ) ); // TODO need to avoid copy construction here
        }
        // TODO print in verbose mode all found plot files?

        // TODO remove duplicate plot files (multiple representations of the same plot file)?
        std::vector<NonceNumRange> overlaps;
        ExecutePairWise( plotFiles.begin(), plotFiles.end(), [&overlaps]
        ( const PlotFile& lhs, const PlotFile& rhs )
        {
            boost::optional<NonceNumRange> possibleOverlap = lhs.CalcNonceRangeIntersectionWith( rhs );
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
            BOOST_LOG_TRIVIAL(error) << "Overlaps found: " << overlaps.at(0);
            // TODO print what exactly intersections are found
        }
    }
};
