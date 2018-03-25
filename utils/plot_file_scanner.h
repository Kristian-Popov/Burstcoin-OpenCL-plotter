#pragma once

#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include "plot_file.h"

class PlotFileScanner
{
public:
    static std::vector<PlotFile> Scan( const std::string& startingDir )
    {
        namespace fs = boost::filesystem;
        fs::recursive_directory_iterator end;
        std::vector<PlotFile> result;
        for( fs::recursive_directory_iterator iter( startingDir ); iter != end; ++iter )
        {
            if ( PlotFile::IsNameValid( iter->path().filename() ) )
            {
                result.push_back( PlotFile( iter->path() ) );
            }
        }
        return result;
    }
};
