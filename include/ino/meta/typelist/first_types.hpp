#pragma once

#include <ino/meta/typelist/List.hpp>

namespace ino {
namespace meta {
namespace typelist {

// Based on: https://stackoverflow.com/a/17508225/

template <std::size_t n, typename In, typename... Out>
struct first_impl;

template <std::size_t n, typename First, typename... Other, typename... Out>
struct first_impl<n, List<First, Other...>, Out...>
{
    using type = typename first_impl<n - 1, List<Other...>, Out..., First>::type; // Move first input to output.
};

//need First, Other... here to resolve ambiguity on n = 0
template <typename First, typename... Other, typename... Out>
struct first_impl<0, List<First, Other...>, Out...>
{
    using type = List<Out...> ;  // Stop if no more elements needed
};

// Explicit rule for empty tuple because of First, Other... in the previous rule.
// actually it's for N == size of tuple
template <typename ... Out>
struct first_impl<0, List<>, Out...>
{
    using type = List<Out...>;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <std::size_t n, typename T>
struct first_types
{
    using type = typename first_impl<n, T>::type;
};

} // namespace typelist
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

#include <type_traits> // for std::is_same

namespace tests {

static_assert(std::is_same< first_types<0, List<> >::type, List<> >::value, "Test failed");

static_assert(std::is_same< first_types<0, List<bool, char, int, float> >::type, List<> >::value, "Test failed");
static_assert(std::is_same< first_types<1, List<bool, char, int, float> >::type, List<bool> >::value, "Test failed");
static_assert(std::is_same< first_types<2, List<bool, char, int, float> >::type, List<bool, char> >::value, "Test failed");
static_assert(std::is_same< first_types<3, List<bool, char, int, float> >::type, List<bool, char, int> >::value, "Test failed");
static_assert(std::is_same< first_types<4, List<bool, char, int, float> >::type, List<bool, char, int, float> >::value, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
