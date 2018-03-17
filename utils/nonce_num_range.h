#pragma once

#include "utils.h"
#include <climits>

class NonceNumRange
{
public:
    NonceNumRange()
    {}

    NonceNumRange( uint64_t startNonceNum, uint64_t sizeInNonce )
        : startNonceNum_( startNonceNum )
        , sizeInNonce_( sizeInNonce )
    {
        EXCEPTION_ASSERT( sizeInNonce_ > 0 );
        //EXCEPTION_ASSERT( ( ULLONG_MAX - sizeInNonce ) > ( startNonceNum - 1 ) ); // TODO this is overflow protection, test it with unit tests
    }

    uint64_t StartNonceNum() const
    {
        return startNonceNum_;
    }

    uint64_t SizeInNonce() const
    {
        return sizeInNonce_;
    }

    uint64_t LastNonceNum() const
    {
        return startNonceNum_ + sizeInNonce_ - 1;
    }

    // Based on https://stackoverflow.com/a/3269442
    bool OverlapsWith( const NonceNumRange& rhs ) const
    {
        return LastNonceNum() >= rhs.StartNonceNum() && StartNonceNum() <= rhs.LastNonceNum();
    }

private:
    uint64_t startNonceNum_ = 0;
    uint64_t sizeInNonce_ = 0;
};
