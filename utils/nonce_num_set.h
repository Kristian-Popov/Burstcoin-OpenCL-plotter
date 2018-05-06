#pragma once

#include "nonce_num_range.h"
#include <vector>
#include <set>

/*
 * Contains a set of nonce numbers (not nessesary contiguous).
 * Does not actually stores all numbers (to save memory)
 */
class NonceNumSet
{
public:
    /*
     * Construct a set from a series of ranges.
     * May contain overlapping nonces (but you mustn't call CutPieceAtBeginning() in this case),
     * but duplicated nonce ranges (exactly identical) are removed
     */
    explicit NonceNumSet( const std::vector<NonceNumRange>& ranges ) // TODO replace with generic iterators or something else?
        : ranges_( ranges.cbegin(), ranges.cend() )
    {}

    std::set<NonceNumRange> GetRanges() const
    {
        return ranges_;
    }

    /*
     * Returns a nonce number set that includes all nonces not included in this one.
     */
    NonceNumSet Inverse() const
    {
        std::vector<NonceNumRange> resultRanges;
        static constexpr uint64_t lastValidNonceNum = ULLONG_MAX - 1;
        // We ignore last nonce number ULLONG_MAX
        // If this set is empty, return all possible nonces
        if ( ranges_.empty() )
        {
            resultRanges.push_back( NonceNumRange::ConstructUsingStartAndEnd( 0, lastValidNonceNum ) );
        }
        else
        {
            // Adding nonces from 0 to first nonce of first range
            if ( ranges_.begin()->StartNonceNum() > 0 )
            {
                resultRanges.push_back( NonceNumRange::ConstructUsingStartAndEnd( 0, ranges_.begin()->StartNonceNum() - 1 ) );
            }

            // If there's more than one range, add all nonces between those ranges
            if ( ranges_.size() > 1 )
            {
                auto firstIter = ranges_.cbegin();
                auto secondIter = std::next( firstIter );
                size_t itersNeeded = std::distance( ranges_.cbegin(), ranges_.cend() ) - 1;

                for ( size_t i = 0; i < itersNeeded; ++i )
                {
                    resultRanges.push_back( NonceNumRange::ConstructUsingStartAndEnd(
                        firstIter->LastNonceNum() + 1,
                        secondIter->StartNonceNum() - 1
                    ) );

                    firstIter++;
                    secondIter++;
                }
            }

            // Adding nonces from last nonce of last range to lastValidNonceNum
            if ( ranges_.rbegin()->LastNonceNum() < lastValidNonceNum )
            {
                resultRanges.push_back( NonceNumRange::ConstructUsingStartAndEnd( ranges_.rbegin()->LastNonceNum() + 1, lastValidNonceNum ) );
            }
        }
        return NonceNumSet( resultRanges );
    }

    /*
     * Cut a piece of this set at a beginning and return it, preferrably with "preferredSplitSize" nonces.
     * Returned nonce range can be smaller than "preferredSplitSize".
     *
     * After calling to this method, set is changed and nonces returned in a result range are no longer part of this set.
     *
     * Set must not contain overlapping nonces.
     */
    NonceNumRange CutPieceAtBeginning( uint64_t preferredSplitSize );
private:
    // We're using a common set here since we need sorted data structure
    std::set<NonceNumRange> ranges_;

    bool AreAnyNoncesOverlapping() const;
};
