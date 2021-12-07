#pragma once

#include <ino/meta/comparison/stl/access_array.hpp>
#include <ino/meta/comparison/stl/acess_initializer_list.hpp>





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

namespace tests {

using namespace ino::meta::comparison;

namespace test_with_pod {

    // Comparison check: POD <--> POD
    static_assert( equal(42, 42) == true, "Test failed");


    // Comparison check: POD <--> std::array
    static_assert( equal(42, std::array<int, 1>{42}) == true, "Test failed");

    // Comparison check: std::array <--> POD
    static_assert( equal(std::array<int, 1>{42}, 42) == true, "Test failed");


    // Comparison check: POD <--> std::initializer_list
    static_assert( equal(42, std::initializer_list<int>{42}) == true, "Test failed");

    // Comparison check: std::initializer_list <--> POD
    static_assert( equal(std::initializer_list<int>{42}, 42) == true, "Test failed");

} // namespace test_with_pod

namespace test_cross {

    // Comparison check: raw array <--> raw array
    static_assert( equal( (std::int32_t[3]){1,2,3}, (std::int32_t[3]){1,2,3}) == true,  "Test failed");
    static_assert( equal( (std::int32_t[3]){1,2,3}, (std::int32_t[3]){1,2,4}) == false, "Test failed"); // Non-equality check
    static_assert( equal( (std::int32_t[2]){1,2},   (std::int32_t[3]){1,2,3}) == false, "Test failed"); // Different sizes check

    // Comparison check: std::array <--> std::array
    static_assert( equal( std::array<std::int32_t, 3>{1,2,3}, std::array<std::int32_t, 3>{1,2,3} ) == true, "Test failed");

    // Comparison check: std::array <--> std::initializer_list
    static_assert( equal( std::array<std::int32_t, 3>{1,2,3}, std::initializer_list<int>{1,2,3} ) == true, "Test failed");

    // Comparison check: std::initializer_list <--> std::array
    static_assert( equal( std::initializer_list<int>{1,2,3}, std::array<std::int32_t, 3>{1,2,3} ) == true, "Test failed");

} // namespace test_cross

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
