#include "utils.h"

#include <type_traits>

#include <boost/format.hpp>

#ifdef __linux__
#   include <sys/sysinfo.h>
#endif

// TODO move these functions to separate project/repository
namespace Utils
{
    std::string ReadFile( const std::string& fileName )
    {
        std::ifstream stream( fileName );
        std::stringbuf buf;
        if( !stream.is_open() )
        {
            throw std::runtime_error( "File not found" );
        }
        stream >> &buf;
        stream.close();
        return buf.str();
    }

    std::string FormatQuantityString( int value )
    {
        std::string result;
        if (value % 1000000 == 0)
        {
            result = std::to_string(value / 1000000) + "M";
        }
        else if( value % 1000 == 0 )
        {
            result = std::to_string( value / 1000 ) + "K";
        }
        else
        {
            result = std::to_string( value);
        }
        return result;
    }

    long double ChooseConvenientUnit( long double value, 
        const std::vector<long double>& units )
    {
        EXCEPTION_ASSERT( !units.empty() );
        EXCEPTION_ASSERT( std::is_sorted( units.begin(), units.end() ) );
        // First the biggest unit that is smaller than input value
        auto resultIter = std::find_if( units.crbegin(), units.crend(),
            [value] (long double unit )
            {
                return unit < value;
            } );
        long double result = 1.0f;
        if ( resultIter == units.crend() )
        {
            result = *units.begin();
        }
        else
        {
            result = *resultIter;
        }
        return result;
    }

    long double ChooseConvenientUnit( const std::vector<long double>& values,
        const std::vector<long double>& units )
    {
        std::vector<long double> convUnits;
        convUnits.reserve( values.size() );

        std::transform( values.begin(), values.end(), std::back_inserter( convUnits ),
            [&units] (long double value)
            {
                return ChooseConvenientUnit(value, units);
            } );

        // Count how many times every unit occurs
        std::unordered_map<long double, int> counts;
        for( long double unit: convUnits )
        {
            ++counts[unit];
        }
        EXCEPTION_ASSERT( !counts.empty() );
        // TODO find a unit that occurs most frequently
		auto resultIter = std::max_element( counts.begin(), counts.end(), 
            CompareSecond<long double, int> );
        return resultIter->first;
    }

    std::string CombineStrings( const std::vector<std::string>& strings, const std::string & delimiter )
    {
        return VectorToString( strings, delimiter );
    }

    // TODO add versions for other OS
    uint64_t CalcAmountOfFreeRAMInBytes()
    {
#ifdef __linux__
        struct sysinfo queryResult;
        EXCEPTION_ASSERT(sysinfo(&queryResult) == 0);
        return queryResult.freeram * queryResult.mem_unit;
#else
        // Entering YOLO mode, return 8 GiB
        return 8 * 1024 * 1024 * 1024; // 8 GiB
#endif
    }
}
