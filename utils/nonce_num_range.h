#pragma once

#include "utils.h"
#include <climits>

#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>

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

    // Based on https://stackoverflow.com/a/42123103
    /*
     * Calculate what nonces can be found in both ranges.
     * If result has no value, ranges do not intersect with each other.
     */
    boost::optional<NonceNumRange> CalcIntersectionWith( const NonceNumRange& rhs ) const
    {
        boost::optional<NonceNumRange> result;
        auto start = std::max( startNonceNum_, rhs.startNonceNum_ );
        auto end = std::min( LastNonceNum(), rhs.LastNonceNum() );
        if ( start <= end )
        {
            result = NonceNumRange( start, end - start + 1 );
        }
        return result;
    }

    bool operator==( const NonceNumRange& rhs ) const
    {
        return this->startNonceNum_ == rhs.startNonceNum_ && this->sizeInNonce_ == rhs.sizeInNonce_;
    }

    bool operator!=( const NonceNumRange& rhs ) const
    {
        return !( *this == rhs );
    }
private:
    uint64_t startNonceNum_ = 0;
    uint64_t sizeInNonce_ = 0;
};

std::ostream& operator<< (std::ostream& stream, const NonceNumRange& range);