#pragma once

#include <type_traits> // for std::true_type, std::conditional

namespace ino {
namespace meta {
namespace traits {

// Port of std::conjuction from C++14
// Based on: https://en.cppreference.com/w/cpp/types/conjunction

template <typename ... Types>
struct conjunction
        : std::true_type
{};

template <typename B1>
struct conjunction<B1>
        : B1
{};

template <typename B1, typename ... Bn>
struct conjunction<B1, Bn...>
    : std::conditional<
        bool(B1::value),
        conjunction<Bn...>,
        B1
    >::type
{};

} // namespace traits
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

namespace tests {

using namespace ino::meta::traits;

template <typename T, typename ... Types>
using _is_all_same = conjunction< std::is_same<T, Types> ...>;

static_assert(_is_all_same<int, int, int>::value  ==  true, "Test failed");
static_assert(_is_all_same<bool, int, int>::value == false, "Test failed");
static_assert(_is_all_same<int, int, bool>::value == false, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
