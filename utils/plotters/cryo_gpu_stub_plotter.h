#pragma once

#include <stdlib.h>

#include <boost/format.hpp>

#include "plotters/plotter_interface.h"

class CryoGPUStubPlotter: public PlotterInterface
{
public:
    void SetParameters( const std::string& strategy, uint64_t staggerSizeInNonces )
    {
        strategy_ = strategy;
        staggerSizeInNonces_ = staggerSizeInNonces;
    }

    void Plot( const std::string& directory, uint64_t accountNumericId, const NonceNumRange& range ) override
    {
        PlotFileParams params( accountNumericId, range, staggerSizeInNonces_ );
        PlotFile plotFile( params, directory );
        std::string command = ( boost::format( "./gpuPlotGenerator generate %s %s" ) %
            strategy_ %
            plotFile.FileNameWithPath() ).str();
        BOOST_LOG_TRIVIAL(info) << "Starting Cryo's GPU plotter, command line: " << command;
        int returnCode = system( command.c_str() );
        BOOST_LOG_TRIVIAL(info) << "Status code of plotter: " << returnCode;
    }

private:
    std::string strategy_;
    uint64_t staggerSizeInNonces_ = 0;
};
