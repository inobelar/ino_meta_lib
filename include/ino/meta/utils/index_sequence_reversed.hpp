#pragma once

#include <ino/meta/utils/index_sequence.hpp>

namespace ino {
namespace meta {
namespace utils {


/**
    Based on: https://stackoverflow.com/a/51409050/

    ----------------------------------------------------------------------------

    @note Original implementation:

    @code{.cpp}
    template <std::size_t ... Indexes>
    constexpr auto index_sequence_reverse(const utils::index_sequence<Indexes...> &)
        -> decltype( utils::index_sequence< (sizeof...(Indexes)-1U) - Indexes...>{} );

    template <std::size_t N>
    using make_index_sequence_reversed
        = decltype(index_sequence_reverse( utils::make_index_sequence<N>{} ));
    @endcode

    produces unexpected results, which pass tests:

    @code{.cpp}
    static_assert(std::is_same<
        make_index_sequence_reversed<0>      , index_sequence<>
        //                      no --> ::type <-- here
    >::value == true, "Test failed" );
    @endcode

    ----------------------------------------------------------------------------
*/

template <typename Sequence>
struct index_sequence_reverse;

template <std::size_t ... Indexes>
struct index_sequence_reverse< index_sequence<Indexes...> >
        : index_sequence< (sizeof...(Indexes)-1U) - Indexes ...>
{ };

// -----------------------------------------------------------------------------

template <std::size_t N>
using make_index_sequence_reversed =
    typename index_sequence_reverse< make_index_sequence<N> >::_type; // TODO: dont use "private" `_type` here

} // namespace utils
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

namespace tests {

using namespace ino::meta::utils;

static_assert(std::is_same<
    make_index_sequence_reversed<0>, index_sequence<>
>::value == true, "Test failed" );

static_assert(std::is_same<
    make_index_sequence_reversed<1U>, index_sequence<0U>
>::value == true, "Test failed" );

static_assert(std::is_same<
    make_index_sequence_reversed<5U>, index_sequence<4U, 3U, 2U, 1U, 0U>
>::value == true, "Test failed" );

// TODO: make it work!
//static_assert(std::is_same<
//    index_sequence_reverse< index_sequence<4, 3, 2, 1> >, index_sequence<1, 2, 3, 4>
//>::value == true, "Test failed" );

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS

