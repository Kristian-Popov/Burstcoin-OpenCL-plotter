#pragma once

#include <stdlib.h>

#include <boost/format.hpp>

#include "plotters/plotter_interface.h"

class CryoGPUStubPlotter final: public PlotterInterface
{
public:
    void SetParameters( const std::string& strategy, uint64_t staggerSizeInNonces )
    {
        strategy_ = strategy;
        maxStaggerSizeInNonces_ = staggerSizeInNonces;
    }

    NonceNumRange Plot( const std::string& directory, uint64_t accountNumericId, const NonceNumRange& prefferedRange ) override
    {
        // TODO move this fitting of range size to stagger to higher level?
        uint64_t nonceNotFittingInStagger = prefferedRange.SizeInNonce() % maxStaggerSizeInNonces_;
        uint64_t staggerSizeInNonces = maxStaggerSizeInNonces_;
        NonceNumRange range = prefferedRange;
        if ( nonceNotFittingInStagger > 0 )
        {
            // If nonce count is smaller than stagger, just use it
            if ( prefferedRange.SizeInNonce() < maxStaggerSizeInNonces_ )
            {
                staggerSizeInNonces = prefferedRange.SizeInNonce();
            }
            else
            {
                // Decrease range size if it doesn't fit stagger perfectly
                uint64_t newRangeSizeInNonce = prefferedRange.SizeInNonce() - nonceNotFittingInStagger;
                range = NonceNumRange( prefferedRange.StartNonceNum(), newRangeSizeInNonce );
                BOOST_LOG_TRIVIAL(warning) << "Reducing range from " << prefferedRange.SizeInNonce() << " nonces to " << newRangeSizeInNonce;
            }
        }

        PlotFileParams params( accountNumericId, range, staggerSizeInNonces );
        PlotFile plotFile( params, directory );
        std::string command = ( boost::format( "./gpuPlotGenerator generate %s %s" ) %
            strategy_ %
            plotFile.FileNameWithPath() ).str();
        BOOST_LOG_TRIVIAL(info) << "Starting Cryo's GPU plotter, command line: " << command;
        int returnCode = system( command.c_str() );
        BOOST_LOG_TRIVIAL(info) << "Status code of plotter: " << returnCode;
        return range;
    }

private:
    std::string strategy_;
    uint64_t maxStaggerSizeInNonces_ = 0;
};
