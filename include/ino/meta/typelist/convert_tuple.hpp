#pragma once

#include <ino/meta/typelist/List.hpp>
#include <tuple>

namespace ino {
namespace meta {
namespace typelist {

template <typename T>
struct list_to_tuple {};

template <typename ... Types>
struct list_to_tuple< List<Types...> > {
    using type = std::tuple<Types...>;
};

// -----------------------------------------------------------------------------

template <typename T>
struct tuple_to_list {};

template <typename ... Types>
struct tuple_to_list< std::tuple<Types ...> > {
    using type = List<Types...>;
};

// -----------------------------------------------------------------------------

template <typename T>
struct list_to_tuple_recursive {
    using type = T;
};

template <typename ... Types>
struct list_to_tuple_recursive< List<Types...> > {
    using type = std::tuple< typename list_to_tuple_recursive<Types>::type ...>;
};


template <typename T>
struct tuple_to_list_recursive {
    using type = T;
};

template <typename ... Types>
struct tuple_to_list_recursive< std::tuple<Types...> > {
    using type = List< typename tuple_to_list_recursive<Types>::type ...>;
};

} // namespace typelist
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

namespace tests {

// List -> std::tuple convertion
static_assert(std::is_same< list_to_tuple< List<> >::type, std::tuple<> >::value, "Test failed");
static_assert(std::is_same< list_to_tuple< List<bool> >::type, std::tuple<bool> >::value, "Test failed");
static_assert(std::is_same< list_to_tuple< List<bool, char> >::type, std::tuple<bool, char> >::value, "Test failed");
static_assert(std::is_same< list_to_tuple< List<bool, char, int> >::type, std::tuple<bool, char, int> >::value, "Test failed");

// std::tuple -> List convertion
static_assert(std::is_same< tuple_to_list< std::tuple<> >::type, List<> >::value, "Test failed");
static_assert(std::is_same< tuple_to_list< std::tuple<bool> >::type, List<bool> >::value, "Test failed");
static_assert(std::is_same< tuple_to_list< std::tuple<bool, char> >::type, List<bool, char> >::value, "Test failed");
static_assert(std::is_same< tuple_to_list< std::tuple<bool, char, int> >::type, List<bool, char, int> >::value, "Test failed");

// -------------------------------------------------------------------------

// Make sure, that non-recursive convertion dont applied into nested types
static_assert(std::is_same< list_to_tuple< List< List<bool, char>, int> >::type, std::tuple< List<bool, char>, int> >::value, "Test failed");
static_assert(std::is_same< tuple_to_list< std::tuple< std::tuple<bool, char>, int> >::type, List< std::tuple<bool, char>, int> >::value, "Test failed");

// Make sure, that recursive convertion applied into nested types
static_assert(std::is_same< list_to_tuple_recursive< List< List<bool, char>, int> >::type, std::tuple< std::tuple<bool, char>, int> >::value, "Test failed");
static_assert(std::is_same< tuple_to_list_recursive< std::tuple< std::tuple<bool, char>, int> >::type, List< List<bool, char>, int> >::value, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
