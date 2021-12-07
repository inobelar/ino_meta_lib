#pragma once

#include <cstddef> // for std::size_t

namespace ino {
namespace meta {
namespace typelist {

template <typename ... Types>
struct List
{
    static constexpr std::size_t count = sizeof...(Types);
};

} // namespace typelist
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

namespace tests {

using namespace ino::meta::typelist;

// List elements count check
static_assert(List<>::count                == 0, "Test failed");
static_assert(List<bool>::count            == 1, "Test failed");
static_assert(List<bool, char>::count      == 2, "Test failed");
static_assert(List<bool, char, int>::count == 3, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
