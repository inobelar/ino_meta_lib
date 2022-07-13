#pragma once

#include <type_traits> // for std::false_type, std::conditional

namespace ino {
namespace meta {
namespace traits {

// Port of std::disjunction from C++14
// Based on: https://en.cppreference.com/w/cpp/types/disjunction

template <typename ... Types>
struct disjunction
        : std::false_type
{};

template <typename B1>
struct disjunction<B1>
        : B1
{};

template <typename B1, typename ... Bn>
struct disjunction<B1, Bn...>
    : std::conditional<
        bool(B1::value),
        B1,
        disjunction<Bn...>
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

// TODO: ADD disjunction tests

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
