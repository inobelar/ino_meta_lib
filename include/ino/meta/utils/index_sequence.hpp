#pragma once

#include <cstddef> // for std::size_t

namespace ino {
namespace meta {
namespace utils {

/*
    Next types area available starting from C++14:
        - `std::integer_sequence`
        - `std::make_integer_sequence`
        - `std::index_sequence`
        - `std::make_index_sequence`

    Implementation based on:
        - https://stackoverflow.com/a/32223343/
            - based on http://stackoverflow.com/a/17426611/410767 by Xeo
        - GCC C++ STL

    Alternate implementation:
        - https://stackoverflow.com/a/49672613/
*/

template <typename T, T ... Indexes>
struct integer_sequence
{
    // `_type` is Non-standard extention, which used in:
    // `detail::_Merge_and_renumber` and `detail::_Make_integer_sequence`
    // For compatibility with c++14 `std::integer_sequence` it must NEVER BE
    // used in public code.
    using _type = integer_sequence;

    using value_type = T;

    static constexpr std::size_t size() noexcept { return sizeof...(Indexes); }
};

// -----------------------------------------------------------------------------

namespace detail {

    template <typename T, typename Sequence1, typename Sequence2>
    struct _Merge_and_renumber;

    // Specialization only for 2 integer_sequence's
    template <typename T, std::size_t... I1, std::size_t... I2>
    struct _Merge_and_renumber<T, integer_sequence<T, I1...>, integer_sequence<T, I2...>>
            : integer_sequence<T, I1..., ((sizeof...(I1))+I2)...>
    {};

    // -------------------------------------------------------------------------

    template <typename T, std::size_t Num>
    struct _Make_integer_sequence
            : _Merge_and_renumber<T,
                typename _Make_integer_sequence<T,       Num/2>::_type,
                typename _Make_integer_sequence<T, Num - Num/2>::_type
            >
    {};

    template <typename T>
    struct _Make_integer_sequence<T, 0>
            : integer_sequence<T>
    {};

    template <typename T>
    struct _Make_integer_sequence<T, 1>
            : integer_sequence<T, 0>
    {};

} // namespace detail

// -----------------------------------------------------------------------------

template <typename T, T Num>
using make_integer_sequence
    = typename detail::_Make_integer_sequence<T, Num>::_type;


template <std::size_t ... Indexes>
using index_sequence = integer_sequence<std::size_t, Indexes ...>;

template <std::size_t Num>
using make_index_sequence = make_integer_sequence<std::size_t, Num>;

template <typename ... Types>
using index_sequence_for = make_index_sequence< sizeof...(Types) >;

} // namespace utils
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

#include <type_traits> // for std::is_same

namespace tests {

using namespace ino::meta::utils;

// Tests for common cases size's
static_assert(index_sequence<>::size() == 0, "Test failed");
static_assert(index_sequence<0>::size() == 1, "Test failed");
static_assert(index_sequence<0, 1, 2, 3, 4>::size() == 5, "Test failed");

// Tests for un-common cases size's
static_assert(index_sequence<10>::size() == 1, "Test failed");
static_assert(index_sequence<5, 8, 3, 7, 6>::size() == 5, "Test failed");
static_assert(index_sequence<3, 3, 3, 3, 3>::size() == 5, "Test failed");


// Tests for sequency generation
static_assert(std::is_same<
    make_index_sequence<0>, index_sequence<>
>::value == true, "Test failed");

static_assert(std::is_same<
    make_index_sequence<1U>, index_sequence<0U>
>::value == true, "Test failed");

static_assert(std::is_same<
    make_index_sequence<5U>, index_sequence<0U, 1U, 2U, 3U, 4U>
>::value == true, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
