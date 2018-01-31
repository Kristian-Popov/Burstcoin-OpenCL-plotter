#include "plot_file_math.h"

namespace
{
    static constexpr uint64_t nonceSizeInBytes_ = 256 * 1024; // 256 KiB
    static constexpr uint64_t scoopSizeInBytes_ = 64;
    static constexpr uint64_t scoopMaxNum_ = 4095;
}

namespace PlotFileMath
{
    uint64_t CalcStaggerCount( const PlotFileParams & params )
    {
        return params.sizeInNonce_ / params.staggerSizeInNonces_;
    }

    // Calculate an offset to the first byte of the stagger
    uint64_t CalcStaggerStartOffsetInBytes( const PlotFileParams & params, uint64_t staggerNum )
    {
        EXCEPTION_ASSERT( staggerNum < CalcStaggerCount( params ) );
        return staggerNum * CalcStaggerSizeInBytes( params );
    }

    uint64_t CalcStaggerSizeInBytes( const PlotFileParams & params )
    {
        return params.staggerSizeInNonces_ * nonceSizeInBytes_;
    }

    // Calculate in which stagger nonce "nonceNum" is located
    uint64_t CalcStaggerNum( const PlotFileParams & params, uint64_t nonceNum )
    {
        EXCEPTION_ASSERT( nonceNum >= params.startNonceNum_ &&
            nonceNum < params.startNonceNum_ + params.sizeInNonce_ );
        uint64_t nonceNumFromFileStart = nonceNum - params.startNonceNum_;
        uint64_t staggerNum = nonceNumFromFileStart / params.staggerSizeInNonces_;
        return staggerNum;
    }

    // Calculate an offset to the first byte of scoop identified by "scoopNum" and "staggerNum"
    // TODO implement class that would hold scoop number and check it is valid (i.e. in range [0; 4095] )
    uint64_t CalcScoopStartOffsetInBytes( const PlotFileParams & params, uint64_t staggerNum, uint64_t scoopNum )
    {
        EXCEPTION_ASSERT( scoopNum <= scoopMaxNum_ );
        uint64_t staggerOffsetInBytes = CalcStaggerStartOffsetInBytes( params, staggerNum );
        uint64_t scoopOffsetInBytes = scoopNum * CalcScoopRegionSizeInBytes( params );
        return staggerOffsetInBytes + scoopOffsetInBytes;
    }

    // "Scoop region" is used here to describe a section of a stagger that holds values
    // of different nonces but with the same scoop number (and in one stagger)
    // Calculate size of a scoop region in byes
    uint64_t CalcScoopRegionSizeInBytes( const PlotFileParams & params )
    {
        return scoopSizeInBytes_ * params.staggerSizeInNonces_;
    }
}
