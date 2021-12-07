#pragma once

#include <ino/meta/utils/index_sequence.hpp>
#include <array>

namespace ino {
namespace meta {
namespace array_utils {

namespace impl {

template <
        typename T, std::size_t ... Indexes,
        std::size_t SIZE = (sizeof...(Indexes)) + 1
>
constexpr std::array<T, SIZE> set_at_impl_first(const T* ptr, const T& value, utils::index_sequence<Indexes...>)
{
    return std::array<T, SIZE> {
        value,
        ptr[Indexes + 1] ...
    };
}

template <
        typename T, std::size_t ... Indexes,
        std::size_t SIZE = (sizeof...(Indexes)) + 1
>
constexpr std::array<T, SIZE> set_at_impl_last(const T* ptr, const T& value, utils::index_sequence<Indexes...>)
{
    return std::array<T, SIZE> {
        ptr[Indexes] ... ,
        value,
    };
}

/*
    Inputs:
    [5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15] <-- array<int, 11> <-- Size = 11
                 ^
                 +----- set_at<4>(42) <-- Index = 4
    <0, 1, 2, 3>   < 0,  1,  2,  3,  4,  5> <-- indexes: IndexesBefore = <0 ... Index-1>, IndexesAfter = <0 ... SIZE-Index-1>

    ----------------------------------------------------------------------------
    Internal indexes recalculation:
    <0, 1, 2, 3>   < 5   6,  7,  8,  9, 10> <-- indexes: IndexesBefore = <0 ... Index-1>, IndexesAfter = <IndexesAfter + (Index + 1)>

    ----------------------------------------------------------------------------
    Result:
    [5, 6, 7, 8] 42[10, 11, 12, 13, 14, 15]
*/
template <
        std::size_t Index,
        typename T,
        std::size_t ... IndexesBefore,
        std::size_t ... IndexesAfter,
        std::size_t SIZE = (sizeof...(IndexesBefore)) + (sizeof...(IndexesAfter)) + 1
>
constexpr std::array<T, SIZE> set_at_impl_middle(const T* ptr, const T& value,
                                               utils::index_sequence<IndexesBefore...>,
                                               utils::index_sequence<IndexesAfter ...>)
{
    return std::array<T, SIZE> {
        ptr[IndexesBefore] ... ,
        value,
        ptr[IndexesAfter + (Index + 1)] ...
    };
}

/*
    Due to nature of compile-time computations, we cannot simply modify in-place
    array of data and return it. In "pure" functional programming we can only
    return a copy of passed data, which contains modified data.
 */
template <
        std::size_t Index,
        typename T, std::size_t SIZE
>
constexpr std::array<T, SIZE> set_at_impl(const T* ptr, const T& value)
{
    static_assert(Index < SIZE, "Index out of bound");

    /*
        NOTE: here we can use only `impl::set_at_impl_middle`, but to make
        compiler's life easier, used simpler specialiations for all special
        cases: {idx==first, idx==last, idx==in_middle}.

        NOTE: here is also possible additional check for another special
        case: modification of single-value array:

        return
                ( (SIZE == 1) && (Index == 0) )
                ?
                (
                    std::array<T, SIZE> { value } // <-- return array with new value
                )
                :
                (
                    // ... Do rest of the logic below ...
                )
     */

    return
            // If
            (
                (Index == 0) // Index == First index
            )
            ? // Then
            (
                  set_at_impl_first(ptr, value, utils::make_index_sequence<SIZE-1>{})
            )
            : // Else
            (
                  // If
                  (
                      (Index == (SIZE-1)) // Index == Last index
                  )
                  ? // Then
                  (
                        set_at_impl_last(ptr, value, utils::make_index_sequence<SIZE-1>{})
                  )
                  : // Else - Index NOT first and NOT last --> index somewhere in the middle
                    (
                        set_at_impl_middle<Index>(ptr, value,
                                                  utils::make_index_sequence<Index>{},
                                                  utils::make_index_sequence<SIZE-Index -1>{})
                  )
            );
}

} // namespace impl



// Overloading for raw data-to-pointer
// TODO: untested
template <
        std::size_t Index,
        typename T, std::size_t SIZE
>
constexpr std::array<T, SIZE> set_at(const T* ptr, const T& value)
{
    return impl::set_at_impl<Index, T, SIZE>(ptr, value);
}

// Overloading for raw array
// TODO: untested
template <
        std::size_t Index,
        typename T, std::size_t SIZE
>
constexpr std::array<T, SIZE> set_at(const T(&array)[SIZE], const T& value)
{
    return impl::set_at_impl<Index, T, SIZE>(array, value);
}

// Overloading for std::array
template <
        std::size_t Index,
        typename T, std::size_t SIZE
>
constexpr std::array<T, SIZE> set_at(const std::array<T, SIZE>& array, const T& value)
{
    return impl::set_at_impl<Index, T, SIZE>( &(array[0]), value);
}

} // namespace array_utils
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

namespace tests {

using namespace ino::meta::array_utils;

    namespace test_single {

        static constexpr std::array<int, 1> ARRAY {10};
        static constexpr auto ARRAY_CHANGED = set_at<0>(ARRAY, 42);

        // Test for changing single-value array
        static_assert( std::get<0>( ARRAY_CHANGED ) == 42, "Test failed" );

    } // namespace test_single

    namespace test_first {

        static constexpr std::array<int, 5> ARRAY {0, 1, 2, 3, 4};
        static constexpr auto ARRAY_CHANGED = set_at<0>(ARRAY, 9);

        // TODO: make tests as equal(ARRAY_CHANGED, EXPECTED)
        static_assert( std::get<0>( ARRAY_CHANGED ) == 9, "Test failed"); // <-- changed
        static_assert( std::get<1>( ARRAY_CHANGED ) == 1, "Test failed");
        static_assert( std::get<2>( ARRAY_CHANGED ) == 2, "Test failed");
        static_assert( std::get<3>( ARRAY_CHANGED ) == 3, "Test failed");
        static_assert( std::get<4>( ARRAY_CHANGED ) == 4, "Test failed");

    } // namespace test_first

    namespace test_last {

        static constexpr std::array<int, 5> ARRAY {0, 1, 2, 3, 4};
        static constexpr auto ARRAY_CHANGED = set_at<4>(ARRAY, 9);

        // TODO: make tests as equal(ARRAY_CHANGED, EXPECTED)
        static_assert( std::get<0>( ARRAY_CHANGED ) == 0, "Test failed");
        static_assert( std::get<1>( ARRAY_CHANGED ) == 1, "Test failed");
        static_assert( std::get<2>( ARRAY_CHANGED ) == 2, "Test failed");
        static_assert( std::get<3>( ARRAY_CHANGED ) == 3, "Test failed");
        static_assert( std::get<4>( ARRAY_CHANGED ) == 9, "Test failed"); // <-- changed

    } // namespace test_last

    namespace test_middle_1 {

        static constexpr std::array<int, 5> ARRAY {0, 1, 2, 3, 4};
        static constexpr auto ARRAY_CHANGED = set_at<1>(ARRAY, 9);

        // TODO: make tests as equal(ARRAY_CHANGED, EXPECTED)
        static_assert( std::get<0>( ARRAY_CHANGED ) == 0, "Test failed");
        static_assert( std::get<1>( ARRAY_CHANGED ) == 9, "Test failed"); // <-- changed
        static_assert( std::get<2>( ARRAY_CHANGED ) == 2, "Test failed");
        static_assert( std::get<3>( ARRAY_CHANGED ) == 3, "Test failed");
        static_assert( std::get<4>( ARRAY_CHANGED ) == 4, "Test failed");

    } // namespace test_middle_1

    namespace test_middle_2 {

        static constexpr std::array<int, 5> ARRAY {0, 1, 2, 3, 4};
        static constexpr auto ARRAY_CHANGED = set_at<2>(ARRAY, 9);

        // TODO: make tests as equal(ARRAY_CHANGED, EXPECTED)
        static_assert( std::get<0>( ARRAY_CHANGED ) == 0, "Test failed");
        static_assert( std::get<1>( ARRAY_CHANGED ) == 1, "Test failed");
        static_assert( std::get<2>( ARRAY_CHANGED ) == 9, "Test failed"); // <-- changed
        static_assert( std::get<3>( ARRAY_CHANGED ) == 3, "Test failed");
        static_assert( std::get<4>( ARRAY_CHANGED ) == 4, "Test failed");

    } // namespace test_middle_2

    namespace test_middle_3 {

        static constexpr std::array<int, 5> ARRAY {0, 1, 2, 3, 4};
        static constexpr auto ARRAY_CHANGED = set_at<3>(ARRAY, 9);

        // TODO: make tests as equal(ARRAY_CHANGED, EXPECTED)
        static_assert( std::get<0>( ARRAY_CHANGED ) == 0, "Test failed");
        static_assert( std::get<1>( ARRAY_CHANGED ) == 1, "Test failed");
        static_assert( std::get<2>( ARRAY_CHANGED ) == 2, "Test failed");
        static_assert( std::get<3>( ARRAY_CHANGED ) == 9, "Test failed"); // <-- changed
        static_assert( std::get<4>( ARRAY_CHANGED ) == 4, "Test failed");

    } // namespace test_middle_3

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS

