#pragma once

#include <ino/meta/utils/index_sequence.hpp>
#include <array>

namespace ino {
namespace meta {
namespace array_utils {

// Based on : https://stackoverflow.com/a/57757301/

namespace impl {

template <typename T, std::size_t ... Indexes,
          std::size_t SIZE = sizeof...(Indexes)>
constexpr std::array<T, SIZE> make_filled_impl(const T& value, utils::index_sequence<Indexes...>) {
    return std::array<T, SIZE> {{
            // Used `Indexes` here to repeat `value` N times.
            // Cast to void here - to supress compiler warning (about unused variable)
            (static_cast<void>(Indexes), value) ...
    }};
}

} // namespace impl

template <std::size_t N, typename T>
constexpr std::array<T, N> make_filled(const T& value) {
    return impl::make_filled_impl<T>(value, utils::make_index_sequence<N>{});
}

} // namespace array_utils
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

namespace tests {

using namespace ino::meta::array_utils;

// This weird case must compiles, why not
constexpr std::array<int, 0> ARRAY_0 = make_filled<0>(42);


// Common cases
constexpr std::array<bool, 1> ARRAY_1 = make_filled<1>(true);
static_assert(ARRAY_1[0] == true, "Test failed");

constexpr std::array<int, 2> ARRAY_2 = make_filled<2>(42);
static_assert(ARRAY_2[0] == 42, "Test failed");
static_assert(ARRAY_2[1] == 42, "Test failed");

constexpr std::array<float, 3> ARRAY_3 = make_filled<3>(3.14f);
static_assert(ARRAY_3[0] == 3.14f, "Test failed");
static_assert(ARRAY_3[1] == 3.14f, "Test failed");
static_assert(ARRAY_3[2] == 3.14f, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
