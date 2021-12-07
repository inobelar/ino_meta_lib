#pragma once

#include <type_traits> // for std::integral_constant

namespace ino {
namespace meta {
namespace traits {

// Based on: https://stackoverflow.com/a/26169248/

template <typename T, typename... Types>
struct index_of;

template <typename T, typename ... Types>
struct index_of<T, T, Types ...>
        : std::integral_constant<std::size_t, 0>
{};

template <typename T, typename Head, typename ... Rest>
struct index_of<T, Head, Rest ...>
        : std::integral_constant<std::size_t, 1 + index_of<T, Rest...>::value>
{};

} // namespace traits
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined(INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

namespace tests {

using namespace ino::meta::traits;

static_assert(index_of<int, int>::value == 0, "Test failed");
static_assert(index_of<bool, bool, char, int>::value == 0, "Test failed");
static_assert(index_of<char, bool, char, int>::value == 1, "Test failed");
static_assert(index_of< int, bool, char, int>::value == 2, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
