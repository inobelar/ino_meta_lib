#pragma once

#include <cstddef> // for std::size_t

namespace ino {
namespace meta {
namespace simple_packet {
namespace traits {

template <std::size_t MaxIdx, std::size_t CurrIdx, typename ... Types>
struct bytes_offset_impl;

// Recursive lookup stop - no types left in type-list
template <std::size_t MaxIdx, std::size_t CurrIdx>
struct bytes_offset_impl<MaxIdx, CurrIdx> {
    constexpr static std::size_t value = 0u;
};

template <std::size_t MaxIdx, std::size_t CurrIdx, typename T, typename ... Types>
struct bytes_offset_impl<MaxIdx, CurrIdx, T, Types ...>
{
    // TODO: inefficient, since compiler recursively looks into both branches.
    //
    // TL;DR: compiler try to evaluate `bytes_offset_impl` even if
    // (CurrIdx >= MaxIdx). It may slow-down compilation time a little bit.
    constexpr static std::size_t value =
            (CurrIdx < MaxIdx) ?
                sizeof(T) + bytes_offset_impl<MaxIdx, (CurrIdx + 1), Types...>::value
            :
                0;
};

template <std::size_t Idx, typename ... Types>
struct bytes_offset {
    constexpr static std::size_t value = bytes_offset_impl<Idx, 0, Types ...>::value;
};

// First type in types-list always have zero offset
template <typename ... Types>
struct bytes_offset<0, Types ...> {
    constexpr static std::size_t value = 0u;
};

} // namespace traits
} // namespace simple_packet
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

#include <cstdint> // for intN_t

namespace tests {

using ino::meta::simple_packet::traits::bytes_offset;

static_assert( bytes_offset<0, int8_t, int16_t, int32_t, float, double>::value ==  0, "Test failed");
static_assert( bytes_offset<1, int8_t, int16_t, int32_t, float, double>::value ==  1, "Test failed");
static_assert( bytes_offset<2, int8_t, int16_t, int32_t, float, double>::value ==  3, "Test failed");
static_assert( bytes_offset<3, int8_t, int16_t, int32_t, float, double>::value ==  7, "Test failed");
static_assert( bytes_offset<4, int8_t, int16_t, int32_t, float, double>::value == 11, "Test failed");

// Test for known-size fixed array
static_assert( bytes_offset<0, int8_t[2], int16_t[3], int32_t[4], float[5], double[6]>::value ==  0, "Test failed");
static_assert( bytes_offset<1, int8_t[2], int16_t[3], int32_t[4], float[5], double[6]>::value ==  2, "Test failed");
static_assert( bytes_offset<2, int8_t[2], int16_t[3], int32_t[4], float[5], double[6]>::value ==  8, "Test failed");
static_assert( bytes_offset<3, int8_t[2], int16_t[3], int32_t[4], float[5], double[6]>::value == 24, "Test failed");
static_assert( bytes_offset<4, int8_t[2], int16_t[3], int32_t[4], float[5], double[6]>::value == 44, "Test failed");

// Uncommon cases
static_assert( bytes_offset<0>::value == 0, "Test failed");
static_assert( bytes_offset<1>::value == 0, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
