#pragma once

#include <stdlib.h>

#include <unordered_map>

#include "plotters/plotter_interface.h"

class CryoGPUStubPlotter: public PlotterInterface
{
public:
    enum class PlotterStrategy
    {
        Buffer,
        Direct
    }

    void SetParameters( PlotterStrategy strategy, uint64_t staggerSize )
    {
        strategy_ = strategy;
        staggerSize_ = staggerSize;
    }

    void Plot( const std::string& directory, uint64_t accountNumericId, const NonceNumRange& range ) override
    {
        PlotFileParams params( accountNumericId, range, staggerSize_ );
        PlotFile plotFile( params, directory );
        std::string command = ( boost::format( "./gpuPlotGenerator generate %s %s" ) %
            strategyNames[strategy_] %
            plotFile.FileNameWithPath() ).str();
        BOOST_LOG_TRIVIAL(info) << "Starting Cryo's GPU plotter, command line: " << command;
        int returnCode = system( command.c_str() );
        BOOST_LOG_TRIVIAL(info) << "Status code of plotter: " << returnCode;
    }

private:
    PlotterStrategy strategy_;
    uint64_t staggerSize_ = 0;
    static const std::unordered_map<PlotterStrategy, std::string> strategyNames = {
        { PlotterStrategy::Buffer, "buffer" },
        { PlotterStrategy::Direct, "direct" }
    };
};
