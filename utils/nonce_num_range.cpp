#include "nonce_num_range.h"

std::ostream& operator<< (std::ostream& stream, const NonceNumRange& range)
{
    stream << "( " << range.StartNonceNum() << " ; " << range.LastNonceNum() << " )";
}
