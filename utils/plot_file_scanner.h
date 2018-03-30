#pragma once

#include <vector>
#include <string>
#include <memory>
#include <boost/filesystem.hpp>
#include "plot_file.h"

class PlotFileScanner
{
public:
    static std::vector<std::unique_ptr<PlotFile>> Scan( const std::string& startingDir )
    {
        // TODO this method is exceptionally slow on large filesystems, optimize it somehow
        namespace fs = boost::filesystem;
        fs::recursive_directory_iterator end;
        std::vector<std::unique_ptr<PlotFile>> result;
        for( fs::recursive_directory_iterator iter( startingDir ); iter != end; ++iter )
        {
            if ( PlotFile::IsNameValid( iter->path().filename() ) )
            {
                result.push_back( std::make_unique<PlotFile>( iter->path() ) );
            }
        }
        return result;
    }
};
