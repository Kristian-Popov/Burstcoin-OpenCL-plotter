#include "nonce_num_set.h"

NonceNumRange NonceNumSet::CutPieceAtBeginning( uint64_t preferredSplitSize )
{
    // TODO make sure that set doesn't have overlaps

    NonceNumRange firstRange = *ranges_.cbegin();
    ranges_.erase( ranges_.cbegin() );

    auto splitResult = firstRange.Split( preferredSplitSize );
    if ( splitResult.second )
    {
        ranges_.insert( *splitResult.second );
    }
    return splitResult.first;
}
