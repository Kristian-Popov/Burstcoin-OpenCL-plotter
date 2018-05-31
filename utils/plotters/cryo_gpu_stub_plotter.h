#pragma once

#include <stdlib.h>

#include <boost/format.hpp>

#include "plotters/plotter_interface.h"

class CryoGPUStubPlotter final: public PlotterInterface
{
public:
    NonceNumRange Plot( const std::string& directory, const PlotFileParams& params, PlottingMode mode ) override
    {
        PlotFile plotFile( params, directory );

        std::string strategy;
        switch ( mode )
        {
            case PlottingMode::Buffer:
                strategy = "buffer";
                break;
            case PlottingMode::Direct:
                strategy = "direct";
                break;
            default:
                throw std::logic_error( "Unsupported plotting mode" );
                break;
        }

        std::string command = ( boost::format( "./gpuPlotGenerator generate %s %s" ) %
            strategy %
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
};
