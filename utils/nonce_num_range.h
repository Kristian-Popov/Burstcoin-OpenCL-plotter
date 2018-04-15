#pragma once

#include "utils.h"
#include <climits>

#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <typeindex>

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
        // Since we're using unsigned values, we can safely permit overflow and easily check if we encounter it
        EXCEPTION_ASSERT( LastNonceNum() >= startNonceNum_ );
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

    /*
     * Like a constructor, but builds a class using start and end nonce numbers.
     */
    static NonceNumRange ConstructUsingStartAndEnd( uint64_t startNonceNum, uint64_t lastNonceNum )
    {
        // TODO move size->last nonce number conversion to a separator function?
        EXCEPTION_ASSERT( lastNonceNum < ULLONG_MAX ); // If we try to use the last nonce number, we'll get overflow in size value
        return NonceNumRange( startNonceNum, lastNonceNum - startNonceNum + 1 );
    }
private:
    uint64_t startNonceNum_ = 0;
    uint64_t sizeInNonce_ = 0;
};

// Specialize std::hash so that NonceNumRange can be used as a key in
// std::unordered_set and std::unordered_map
namespace std
{
    template <> struct hash<NonceNumRange>
    {
        std::size_t operator()( const NonceNumRange& t ) const
        {
            // Based on https://stackoverflow.com/a/17017281
            return t.StartNonceNum() ^ ( t.SizeInNonce() << 1 );
        }
    };
}

std::ostream& operator<< (std::ostream& stream, const NonceNumRange& range);

bool operator<(const NonceNumRange& lhs, const NonceNumRange& rhs);
