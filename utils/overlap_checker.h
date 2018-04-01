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
        std::vector<std::shared_ptr<PlotFile>> plotFiles;
        for (const std::string& dir: directories)
        {
            Utils::AppendVectorToVector( plotFiles, PlotFileScanner::Scan( dir ) );
        }

        for ( const std::shared_ptr<PlotFile>& plotFile: plotFiles )
        {
            BOOST_LOG_TRIVIAL(debug) << "Found plot file at path " << plotFile->FileNameWithPath();

            // Verify that all pointers are not empty
            EXCEPTION_ASSERT( plotFile );
        }

        std::sort( plotFiles.begin(), plotFiles.end() );
        plotFiles.erase( std::unique( plotFiles.begin(), plotFiles.end() ), plotFiles.end() );

        // Store information about overlaps and files where they are
        std::unordered_map<NonceNumRange, std::vector<std::shared_ptr<PlotFile>>> overlapInfo;
        Utils::ExecutePairWise( plotFiles.begin(), plotFiles.end(), [&overlapInfo]
        ( std::shared_ptr<PlotFile> lhs, std::shared_ptr<PlotFile> rhs )
        {
            boost::optional<NonceNumRange> possibleOverlap = lhs->CalcNonceRangeIntersectionWith( *rhs );
            if ( possibleOverlap )
            {
                std::vector<std::shared_ptr<PlotFile>>& plotFileVector = overlapInfo[possibleOverlap.value()];
                plotFileVector.push_back( lhs );
                plotFileVector.push_back( rhs );
            }
        } );

        // Remove duplicate references to plot files in "overlapInfo"
        for ( auto& overlap: overlapInfo )
        {
            std::vector<std::shared_ptr<PlotFile>>& plotFileVector = overlap.second;
            std::sort( plotFileVector.begin(), plotFileVector.end() );
            plotFileVector.erase( std::unique( plotFileVector.begin(), plotFileVector.end() ), plotFileVector.end() );
        }

        // Log results
        if ( overlapInfo.empty() )
        {
            BOOST_LOG_TRIVIAL(info) << "No overlaps found";
        }
        else
        {
            std::stringstream stream;
            for ( auto& overlap: overlapInfo )
            {
                stream << "Overlap found: " << overlap.first << std::endl;
                for ( const auto& plotFilePtr: overlap.second )
                {
                    stream << "\t in file " << plotFilePtr->FileNameWithPath() << std::endl;
                }
                BOOST_LOG_TRIVIAL(error) << stream.str();
                stream.clear();
            }
        }
    }
};
