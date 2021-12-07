#pragma once

#include <type_traits> // for std::is_same

namespace ino {
namespace meta {
namespace traits {

// Based on: https://stackoverflow.com/a/41932340/

template <typename ... Types>
struct is_unique;

template <typename T, typename Head, typename ... Rest>
struct is_unique<T, Head, Rest ...>
{
    static constexpr bool value =
            is_unique<T, Head>::value &&
            is_unique<T, Rest ...>::value &&
            is_unique<Head, Rest ...>::value;
};

// Recursion stop - checking single 2 values
template <typename T, typename U>
struct is_unique<T, U>
{
    static constexpr bool value = !std::is_same<T, U>::value;
};

// Specialization for single type in parameter pack
template <typename T>
struct is_unique<T>
{
    static constexpr bool value = true;
};

} // namespace traits
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

namespace tests {

using namespace ino::meta::traits;

// Special check, to make sure, that single type allowed
static_assert(is_unique<int>::value == true, "Test failed");

static_assert(is_unique<bool, char, int>::value == true, "Test failed");
static_assert(is_unique<bool, int, int>::value == false, "Test failed");
static_assert(is_unique<int, int, bool>::value == false, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
