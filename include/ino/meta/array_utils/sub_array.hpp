#pragma once

#include <ino/meta/utils/index_sequence.hpp>
#include <array>

namespace ino {
namespace meta {
namespace array_utils {

namespace impl {

template <
        std::size_t Index = 0,
        typename T, std::size_t IN_SIZE,

        std::size_t ... Indexes,
        std::size_t OUT_SIZE = sizeof...(Indexes)
>
constexpr std::array<T, OUT_SIZE> sub_array_ptr_impl(const T* ptr, utils::index_sequence<Indexes...>)
{
    static_assert( (Index + OUT_SIZE) <= IN_SIZE, "Out of bound access");
    return std::array<T, OUT_SIZE>{ ptr[(Index + Indexes)] ... };
}

} // namespace impl

// Overloading for raw pointer-to-data
// TODO: untested
template <
        std::size_t Index, std::size_t Count,
        typename T, std::size_t SIZE
        >
constexpr std::array<T, Count> sub_array(const T* ptr)
{
    return impl::sub_array_ptr_impl<Index, T, SIZE>( ptr, utils::make_index_sequence<Count>{});
}

// Overloading for raw array
// TODO: untested
template <
        std::size_t Index, std::size_t Count,
        typename T, std::size_t SIZE
        >
constexpr std::array<T, Count> sub_array(const T(&array)[SIZE])
{
    return impl::sub_array_ptr_impl<Index, T, SIZE>( array, utils::make_index_sequence<Count>{});
}

// Overloading for std::array
template <
        std::size_t Index, std::size_t Count,
        typename T, std::size_t SIZE
        >
constexpr std::array<T, Count> sub_array(const std::array<T, SIZE>& array)
{
    return impl::sub_array_ptr_impl<Index, T, SIZE>( &(array[0]), utils::make_index_sequence<Count>{});
}

} // namespace array_utils
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

namespace tests {

using namespace ino::meta::array_utils;

// TODO: ADD TESTS
static_assert( std::get<0>( sub_array<0, 2>( std::array<int, 3> {1, 2, 3} ) ) == 1, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
