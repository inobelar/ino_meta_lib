#pragma once

#include <ino/meta/traits/disjunction.hpp>

namespace ino {
namespace meta {
namespace traits {

/**
    Based on:
      https://stackoverflow.com/a/57528226/
      https://stackoverflow.com/a/62089731/

    Additional references:
      https://stackoverflow.com/a/62108334/

      https://stackoverflow.com/a/52092696/

    # Example of usage

    @code{.cpp}
    template <typename ... Types>
    using unique_tuple = typename unique<std::tuple<>, Types ...>::type;

    unique_tuple<bool, int, float, int, int, float> tuple; // --> std::tuple<bool, int, float>

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    template <typename ... Types>
    using unique_variant = typename unique<std::variant<>, Types...>::type;

    unique_variant<bool, int, float, int, int, float> var; // --> std::variant<bool, int, float>
    @endcode
*/


/**
    End of recursive call: tuple is forwared using `type`

    Note: In C++20 it may be implemented like:

    @code{.cpp}
    template <typename T, typename ... Types>
    struct unique : std::type_identity<T> {};
    @endcode
*/
template <typename T, typename ... Types>
struct unique {
    using type = T;
};

/**
    Recursive call:
      1. Consumes the first type of the variadic arguments,
         if not repeated add it to the tuple.
      2. Call this again with the rest of arguments

    Note: In C++17 it may be implemented by using fold-expression:

    @code{.cpp}
    template <
        template<class...> class Tuple, typename... TupleTypes,
        typename U, typename... Us
    >
    struct unique<Tuple<TupleTypes...>, U, Us...>
        : std::conditional<
            (std::is_same_v<U, TupleTypes> || ...)
            , unique_impl<Tuple<TupleTypes...>, Us...>
            , unique_impl<Tuple<TupleTypes..., U>, Us...>>::type
    {};
    @endcode
*/
template <
        template<class...> class Tuple, typename... TupleTypes,
        typename U, typename... Us
>
struct unique< Tuple<TupleTypes...> , U, Us...>
    : std::conditional<
        disjunction< std::is_same<U, TupleTypes> ... >::value
        , unique<Tuple<TupleTypes...>, Us...>
        , unique<Tuple<TupleTypes..., U>, Us...>
     >::type
{};

} // namespace traits
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

#include <tuple>   // for std::tuple<Types...>
#include <cstdint> // for std::int8_t, std::int32_t, std::int64_t

namespace tests {

using namespace ino::meta::traits;

// No-types check
static_assert(std::is_same<
    unique< std::tuple<> >::type,
    std::tuple<>
>::value == true, "Test failed");

// Single type check
static_assert(std::is_same<
    unique< std::tuple<>, bool >::type,
    std::tuple<bool>
>::value == true, "Test failed");

// No-duplicates check
static_assert(std::is_same<
    unique< std::tuple<>, bool, int, float >::type,
    std::tuple<bool, int, float>
>::value == true, "Test failed");

// Single duplicate check
static_assert(std::is_same<
    unique< std::tuple<>, int, int>::type,
    std::tuple<int>
>::value == true, "Test failed");

// Multiple duplicates check
static_assert(std::is_same<
    unique< std::tuple<>, int, int, int, int, int >::type,
    std::tuple<int>
>::value == true, "Test failed");

// Few duplicates check
static_assert(std::is_same<
    unique< std::tuple<>, int, int,    float, float >::type,
    std::tuple<int, float>
>::value == true, "Test failed");

// Duplicates in the middle check
static_assert(std::is_same<
    unique< std::tuple<>, bool,    int, int, int, float, float, float,    double >::type,
    std::tuple<bool, int, float, double>
>::value == true, "Test failed");

// Duplicates mixed check
static_assert(std::is_same<
    unique< std::tuple<>, bool,    int, int, int,    float,   double, double,    int,    bool, bool>::type,
    std::tuple<bool, int, float, double>
>::value == true, "Test failed");

// Duplicates mixed check
static_assert(std::is_same<
    unique< std::tuple<>, bool, int, float,    bool, int, float >::type,
    std::tuple<bool, int, float>
>::value == true, "Test failed");

// Duplicated type-alias check
static_assert(std::is_same<
    unique< std::tuple<>, short, int, std::int16_t,    std::int32_t >::type,
    std::tuple<short, int>
>::value == true, "Test failed");

// Duplicated type-alias check
static_assert(std::is_same<
    unique< std::tuple<>, signed char, std::int8_t >::type,
    std::tuple<signed char>
>::value == true, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS

