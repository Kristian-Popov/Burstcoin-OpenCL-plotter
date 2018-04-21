#include "nonce_num_range.h"

std::ostream& operator<< (std::ostream& stream, const NonceNumRange& range)
{
    stream << "( " << range.StartNonceNum() << " ; " << range.LastNonceNum() << " )";
}

bool operator<(const NonceNumRange& lhs, const NonceNumRange& rhs)
{
    // Nonce range are considered smaller if it starts with a smaller nonce
    if ( lhs.StartNonceNum() != rhs.StartNonceNum() )
    {
        return lhs.StartNonceNum() < rhs.StartNonceNum();
    }
    // If start nonces are equal, smaller nonce has smaller size
    return lhs.SizeInNonce() < rhs.SizeInNonce();
}

NonceNumRange::SplitResult NonceNumRange::Split( uint64_t preferredSplitSize ) const
{
    NonceNumRange::SplitResult result;
    if ( sizeInNonce_ <= preferredSplitSize )
    {
        result.first = *this;
    }
    else
    {
        result.first = NonceNumRange( startNonceNum_, preferredSplitSize );
        result.second = NonceNumRange( startNonceNum_ + preferredSplitSize, sizeInNonce_ - preferredSplitSize );
    }
    return result;
}

