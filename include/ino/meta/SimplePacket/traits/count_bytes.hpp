#pragma once

#include <cstddef> // for std::size_t

namespace ino {
namespace meta {
namespace simple_packet {
namespace traits {

// via: https://stackoverflow.com/a/68284102/

template <typename... Args>
struct count_bytes;

// Recursion stop
template <>
struct count_bytes<> {
    constexpr static std::size_t value = 0u;
};

// Recursive sum
template <typename T, typename... Args>
struct count_bytes<T, Args...> {
    constexpr static std::size_t value = sizeof(T) + count_bytes<Args...>::value;
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

using ino::meta::simple_packet::traits::count_bytes;

// Compile-time testing
static_assert(count_bytes<int8_t, int16_t>::value             ==  3, "Test failed");
static_assert(count_bytes<int8_t, int16_t, int32_t>::value    ==  7, "Test failed");
static_assert(count_bytes<float, float, float, double>::value == 20, "Test failed");

// Test for known-size fixed array
static_assert(count_bytes<int, int[2]>::value                 == 12, "Test failed");

// Attention: sizeof(void) works for C, but not for C++. Reference: https://stackoverflow.com/a/1666232/
// static_assert(count_bytes<void>::value                     ==  1, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
