#pragma once

#include "utils.h"
#include "plot_file_params.h"

namespace PlotFileMath
{
    uint64_t GetNonceSizeInBytes();

    // Calculate how many staggers are needed to fit the data using supplied parameters
    uint64_t CalcStaggerCount( const PlotFileParams & params );

    // Calculate an offset to the first byte of the stagger
    uint64_t CalcStaggerStartOffsetInBytes( const PlotFileParams& params, uint64_t staggerNum );

    uint64_t CalcStaggerSizeInBytes( const PlotFileParams& params );

    // Calculate in which stagger nonce "nonceNum" is located
    //uint64_t CalcStaggerNum( const PlotFileParams& params, uint64_t nonceNum );

    // Calculate an offset to the first byte of scoop identified by "scoopNum" and "staggerNum"
    // TODO implement class that would hold scoop number and check it is valid (i.e. in range [0; 4095] )
    uint64_t CalcScoopStartOffsetInBytes( const PlotFileParams& params, uint64_t staggerNum, uint64_t scoopNum );

    // "Scoop region" is used here to describe a section of a stagger that holds values
    // of different nonces but with the same scoop number (and in one stagger)
    // Calculate size of a scoop region in byes
    uint64_t CalcScoopRegionSizeInBytes( const PlotFileParams& params );

    // Calculate plot file size based on supplied parameters
    uint64_t CalcPlotFileSize( const PlotFileParams& params );
}
