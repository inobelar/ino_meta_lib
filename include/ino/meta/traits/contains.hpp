#pragma once

#include <type_traits> // for std::true_type, std::conditional, std::is_same

namespace ino {
namespace meta {
namespace traits {

// Based on: https://stackoverflow.com/a/2121434/

template <typename T, typename ... Types>
struct contains
        : std::true_type
{};

template <typename T, typename Head, typename... Rest>
struct contains<T, Head, Rest ...>
    : std::conditional<
        std::is_same<T, Head>::value,
        std::true_type,
        contains<T, Rest ...>
    >::type
{};

template <typename T>
struct contains<T>
        : std::false_type
{};

} // namespace traits
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

namespace tests {

using namespace ino::meta::traits;

static_assert(contains<bool, bool, int, float>::value == true,  "Test failed");
static_assert(contains<char, bool, int, float>::value == false, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
