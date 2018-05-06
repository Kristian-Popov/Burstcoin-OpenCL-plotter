#include "nonce_num_set.h"

NonceNumRange NonceNumSet::CutPieceAtBeginning( uint64_t preferredSplitSize )
{
    // TODO make sure that set doesn't have overlaps
    EXCEPTION_ASSERT( !AreAnyNoncesOverlapping() );

    NonceNumRange firstRange = *ranges_.cbegin();
    ranges_.erase( ranges_.cbegin() );

    auto splitResult = firstRange.Split( preferredSplitSize );
    if ( splitResult.second )
    {
        ranges_.insert( *splitResult.second );
    }
    return splitResult.first;
}

bool NonceNumSet::AreAnyNoncesOverlapping() const
{
    // TODO resembles part in OverlapChecker::CheckForNonceOverlaps(), but with differences.
    // No idea if they can be reworked to a single piece of code
    bool result = false;
    Utils::ExecutePairWise( ranges_.begin(), ranges_.end(), [&result]
        ( const NonceNumRange& lhs, const NonceNumRange& rhs )
        {
            boost::optional<NonceNumRange> possibleOverlap = lhs.CalcIntersectionWith( rhs );
            result = result || possibleOverlap;
        }
    );

    return result;
}
