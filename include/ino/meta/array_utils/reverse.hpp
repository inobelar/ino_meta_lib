#pragma once

#include <ino/meta/utils/index_sequence.hpp>
#include <array>

namespace ino {
namespace meta {
namespace array_utils {

namespace impl {

// NOTE: alternatively, instead of manual indexes reversing in `revere_impl`,
// we can use `utils::make_index_sequence_reversed` with already-reversed
// indexes.
template <
        typename T, std::size_t ... Indexes,
        std::size_t SIZE = sizeof...(Indexes)
        >
constexpr std::array<T, SIZE> reverse_ptr_impl(const T* ptr, utils::index_sequence<Indexes...>)
{
    return std::array<T, SIZE>{ ptr[ (SIZE - 1) - Indexes ] ... };
}

} // namespace impl



// Overloading for raw pointer-to-data
// TODO: untested
template <typename T, std::size_t SIZE>
constexpr std::array<T, SIZE> reverse(const T* ptr)
{
    return impl::reverse_ptr_impl(ptr, utils::make_index_sequence<SIZE>{});
}

// Overloading for raw array
// TODO: untested
template <typename T, std::size_t SIZE>
constexpr std::array<T, SIZE> reverse(const T(&array)[SIZE])
{
    return impl::reverse_ptr_impl(array, utils::make_index_sequence<SIZE>{});
}

// Overloading for std::array
template <typename T, std::size_t SIZE>
constexpr std::array<T, SIZE> reverse(const std::array<T, SIZE>& array)
{
    return impl::reverse_ptr_impl(&(array[0]), utils::make_index_sequence<SIZE>{});
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

        static constexpr std::array<int, 1> ARRAY {42};
        static constexpr auto ARRAY_REVERSED = reverse(ARRAY);

        // Test for reversing single-value array
        static_assert( std::get<0>( ARRAY_REVERSED ) == 42, "Test failed");

    } // namespace test_single

    namespace test_odd {

        static constexpr std::array<int, 5> ARRAY {0, 1, 2, 3, 4};
        static constexpr auto ARRAY_REVERSED = reverse(ARRAY);

        // Tests for reversion of array with an ODD values count
        static_assert( std::get<0>( ARRAY_REVERSED ) == 4, "Test failed");
        static_assert( std::get<1>( ARRAY_REVERSED ) == 3, "Test failed");
        static_assert( std::get<2>( ARRAY_REVERSED ) == 2, "Test failed");
        static_assert( std::get<3>( ARRAY_REVERSED ) == 1, "Test failed");
        static_assert( std::get<4>( ARRAY_REVERSED ) == 0, "Test failed");

    } // namespace test_odd

    namespace test_even {

        static constexpr std::array<int, 6> ARRAY {0, 1, 2, 3, 4, 5};
        static constexpr auto ARRAY_REVERSED = reverse(ARRAY);

        // Tests for reversion of array with an EVEN values count
        static_assert( std::get<0>( ARRAY_REVERSED ) == 5, "Test failed");
        static_assert( std::get<1>( ARRAY_REVERSED ) == 4, "Test failed");
        static_assert( std::get<2>( ARRAY_REVERSED ) == 3, "Test failed");
        static_assert( std::get<3>( ARRAY_REVERSED ) == 2, "Test failed");
        static_assert( std::get<4>( ARRAY_REVERSED ) == 1, "Test failed");
        static_assert( std::get<5>( ARRAY_REVERSED ) == 0, "Test failed");

    } // namespace test_even

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
