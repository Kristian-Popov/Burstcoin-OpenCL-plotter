#pragma once

#include <stdlib.h>

#include <boost/format.hpp>

#include "plotters/plotter_interface.h"

class CryoGPUStubPlotter final: public PlotterInterface
{
public:
    void SetParameters( const std::string& strategy )
    {
        strategy_ = strategy;
    }

    NonceNumRange Plot( const std::string& directory, const PlotFileParams& params ) override
    {
        PlotFile plotFile( params, directory );
        std::string command = ( boost::format( "./gpuPlotGenerator generate %s %s" ) %
            strategy_ %
            plotFile.FileNameWithPath() ).str();
        BOOST_LOG_TRIVIAL(info) << "Starting Cryo's GPU plotter, command line: " << command;
        int returnCode = system( command.c_str() );
        if ( returnCode == EXIT_SUCCESS )
        {
            BOOST_LOG_TRIVIAL(info) << "Status code of plotter: " << returnCode << ", probably successfull run";
        }
        else
        {
            BOOST_LOG_TRIVIAL(error) << "Status code of plotter: " << returnCode << ", looks like an error";
        }
        return params.nonceNumRange_;
    }

private:
    std::string strategy_;
};
