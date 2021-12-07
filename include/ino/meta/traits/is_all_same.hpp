#pragma once

#include <ino/meta/traits/conjunction.hpp>

namespace ino {
namespace meta {
namespace traits {

// Based on: https://stackoverflow.com/a/54040525/

template <typename T, typename ... Types>
using is_all_same = conjunction< std::is_same<T, Types> ...>;

} // namespace traits
} // namespace meta
} // namespace ino




// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

namespace tests {

using namespace ino::meta::traits;

static_assert(is_all_same<int, int, int>::value == true, "Test failed");
static_assert(is_all_same<bool, int, int>::value == false, "Test failed");
static_assert(is_all_same<int, int, bool>::value == false, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
