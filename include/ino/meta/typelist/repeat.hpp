#pragma once

#include <ino/meta/typelist/join.hpp>

namespace ino {
namespace meta {
namespace typelist {

// via: https://stackoverflow.com/a/33511913/

template<typename T, std::size_t N>
struct repeat
{
    using left  = typename repeat<T, N/2>::type;
    using right = typename repeat<T, N/2 + N%2>::type;
    using type  = typename join<left, right>::type;
};


// Explicit specialization for single type (to stop recursion)
template <typename T>
struct repeat<T, 1>
{
    using type = List<T>;
};

// Explicit specialization for none type (to stop recursion)
template <typename T>
struct repeat<T, 0>
{
    using type = List<>;
};

} // namespace typelist
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

#include <type_traits> // for std::is_same

namespace tests {

using namespace ino::meta::typelist;

// Test for generating empty typeist (zero size)
static_assert(std::is_same< repeat<int, 0>::type, List<> >::value, "Test failed");

// Test for generating single-type typelist
static_assert(std::is_same< repeat<int, 1>::type, List<int> >::value, "Test failed");

// Test for generating typelist with N same types
static_assert(std::is_same< repeat<int, 2>::type, List<int, int> >::value, "Test failed");
static_assert(std::is_same< repeat<int, 3>::type, List<int, int, int> >::value, "Test failed");
static_assert(std::is_same< repeat<int, 4>::type, List<int, int, int, int> >::value, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
