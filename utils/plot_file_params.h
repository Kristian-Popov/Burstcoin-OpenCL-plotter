#pragma once

#include <cstdint>
#include "nonce_num_range.h"

struct PlotFileParams
{
    PlotFileParams()
    {}

    PlotFileParams(
        uint64_t accountNumericId,
        uint64_t startNonceNum,
        uint64_t sizeInNonce,
        uint64_t staggerSizeInNonces )
        : nonceNumRange_( startNonceNum, sizeInNonce )
        , accountNumericId_( accountNumericId )
        , staggerSizeInNonces_( staggerSizeInNonces )
    {
    }

    PlotFileParams(
        uint64_t accountNumericId,
        const NonceNumRange& nonceNumRange,
        uint64_t staggerSizeInNonces )
        : nonceNumRange_( nonceNumRange )
        , accountNumericId_( accountNumericId )
        , staggerSizeInNonces_( staggerSizeInNonces )
    {
    }

    uint64_t staggerSizeInNonces_ = 0, accountNumericId_ = 0;
    NonceNumRange nonceNumRange_;
};
