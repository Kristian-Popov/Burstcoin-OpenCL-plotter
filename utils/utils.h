#pragma once

#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <functional>
#include <memory>
#include <unordered_map>

namespace Utils
{
    std::string ReadFile( const std::string& fileName );

    template <typename T>
    std::string VectorToString( const std::vector<T>& v, const std::string& delimiter = ", " )
    {
        std::stringstream result;
        std::copy( v.begin(), v.end(), std::ostream_iterator<T>( result, delimiter.c_str() ) );
        return result.str();
    }

    template <typename T>
    void AppendVectorToVector( T& v1, const T& v2 )
    {
        v1.insert( v1.end(), v2.begin(), v2.end() );
    }

    template <typename T>
    void AppendVectorToVector( T& v1, T&& v2 )
    {
        // TODO improve, probably this variant is slow
        v1.reserve( v1.size() + v2.size() );
        for ( auto&& val: v2 )
        {
            v1.push_back( std::move( val ) );
        }
    }

    /*
        Formats text representation of large values:
        - for 1-1000, returned as is
        - for 1000-999000 when last three digits are zeroes - 1K, 999K
        - for 1000000-999000000 when last six digits are zeroes - 1M, 999M
    */
    std::string FormatQuantityString(int value);

    /*
    Choose a unit for value from a supplied list convenient for output.
    Preferred range of values suitable for output starts at 1, as small as possible.
    "units" range must be sorted and must not be empty
    */
    long double ChooseConvenientUnit(long double value, 
        const std::vector<long double>& units);

    /*
    Choose a unit for a list of values from a supplied list convenient for output.
    Preferred range of values suitable for output starts at 1, as small as possible.
    "units" range must be sorted and must not be empty.
    Values may have different suitable units, function selects a unit
    that is convenient for as many values as possible.
    */
    long double ChooseConvenientUnit( const std::vector<long double>& values,
        const std::vector<long double>& units );

    /*
        Compare functor that performs comparison of two pairs by first element
    */
    template<typename T, typename U>
    bool CompareFirst(const std::pair<T, U>& lhs, const std::pair<T, U>& rhs)
    {
        return lhs.first < rhs.first;
    }

    /*
    Compare functor that performs comparison of two pairs by second element
    */
    template<typename T, typename U>
    bool CompareSecond( const std::pair<T, U>& lhs, const std::pair<T, U>& rhs )
    {
        return lhs.second < rhs.second;
    }

    /*
        A functor that returns first element of a pair
    */
    template<typename T, typename U>
    T SelectFirst( const std::pair<T, U>& p )
    {
        return p.first;
    }

    /*
        A functor that returns second element of a pair
    */
    template<typename T, typename U>
    U SelectSecond( const std::pair<T, U>& p )
    {
        return p.second;
    }

    std::string CombineStrings( const std::vector<std::string>& strings, const std::string & delimiter = "\n" );

    /*
        Tries to get a value from a map by key. If it is not found, use a default one supplied as a parameter
    */
    template<typename C, typename K, typename V>
    V FindInMapWithDefault( const C& container, K key, V defaultValue )
    {
        V result;
        auto iter = container.find( key );
        if( iter != container.end() )
        {
            result = iter->second;
        }
        else
        {
            result = defaultValue;
        }
        return result;
    }

    uint64_t CalcAmountOfFreeRAMInBytes();
}

// Based on https://codereview.stackexchange.com/a/23287
/*
 * Execute a function "func" on all pairs in range [first, last)
 * Each value combines with every other value. None combine with themselves and symmetric pairings,
 * i.e. function is not executed on the same value and on a symmetric pair (e.g. (1, 2) / (2, 1) )
 */
template<typename Iter, typename Func>
void ExecutePairWise(Iter first, Iter last, Func func)
{
    for(; first != last; ++first)
    {
        for(Iter next = std::next(first); next != last; ++next)
        {
            func(*first, *next);
        }
    }
}

#define EXCEPTION_ASSERT(expr) { if(!(expr)) { throw std::logic_error("Assert \"" #expr "\" failed"); } }
