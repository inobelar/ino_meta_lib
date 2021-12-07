#pragma once

#include <ino/meta/traits/conjunction.hpp>

namespace ino {
namespace meta {
namespace variant_enum {
namespace traits {

// Inspired by: https://stackoverflow.com/a/54040525/
template <typename ... Enums>
struct is_all_enums {
    static constexpr bool value =
            ino::meta::traits::conjunction<
                std::is_enum<Enums> ...
            >::value;
};

} // namespace traits
} // namespace variant_enum
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

namespace tests {

using namespace ino::meta::variant_enum::traits;

enum       __E1 { };
enum class __E2 { };
enum class __E3 { };

static_assert( is_all_enums<__E1, __E2, __E3>::value == true, "Test failed");
static_assert( is_all_enums<__E1, __E2, int>::value == false, "Test failed");
static_assert( is_all_enums<bool, char, int>::value == false, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
