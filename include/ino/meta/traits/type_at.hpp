#pragma once

#include <cstddef> // for std::size_t

namespace ino {
namespace meta {
namespace traits {

/**
    Based on: https://stackoverflow.com/a/20163490/

    Renamed from original `tuple_element` name into `type_at`, since it not
    about tuples, but about type-lists.

    Note: this is kinda-lighweight version of `std::tuple_element`, and may be
    implemented much simpler (https://stackoverflow.com/a/29753388/):
    @code{.cpp}
        template<std::size_t Index, typename... Types>
        using type_at
            = typename std::tuple_element<Index, std::tuple<Types...> >::type;
    @endcode
*/

template <std::size_t Index, typename ... Types>
struct type_at;

// End of recursion
template <typename T, typename... Rest>
struct type_at<0, T, Rest...> {
    using type = T;
};

// Recursively traverse types
template <std::size_t Index, typename Head, typename... Rest>
struct type_at<Index, Head, Rest...> {
    using type = typename type_at<Index-1, Rest...>::type;
};

} // namespace traits
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

#include <type_traits> // for std::is_same

namespace tests {

using namespace ino::meta::traits;

// Test is single type allowed
static_assert(std::is_same< type_at<0, int>::type, int >::value == true, "Test failed");

static_assert(std::is_same< type_at<0, bool, char, int>::type, bool >::value == true, "Test failed");
static_assert(std::is_same< type_at<1, bool, char, int>::type, char >::value == true, "Test failed");
static_assert(std::is_same< type_at<2, bool, char, int>::type, int  >::value == true, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
