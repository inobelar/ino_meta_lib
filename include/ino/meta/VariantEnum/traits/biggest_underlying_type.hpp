#pragma once

#include <type_traits> // for std::conditional, std::underlying_type, std::is_enum

namespace ino {
namespace meta {
namespace variant_enum {
namespace traits {

template <typename ... Enums>
struct biggest_underlying_type {};

template <typename Enum, typename Head, typename ... Rest>
struct biggest_underlying_type<Enum, Head, Rest ...>
{
    using type = typename
        // Compare first enum ...
        biggest_underlying_type<Enum,
            // ... with result of recursive nested comparison
            typename biggest_underlying_type<Head, Rest ...>::type
    >::type;
};

// Recursion stop case
template <typename EnumA, typename EnumB>
struct biggest_underlying_type<EnumA, EnumB>
{
    using type = typename std::conditional<
        (
            sizeof( typename std::underlying_type<EnumA>::type ) >=
            sizeof( typename std::underlying_type<EnumB>::type )
        ),
        EnumA, // <-- If EnumA >= EnumB
        EnumB  // <-- If EnumA <  EnumB
    >::type;
};

// Specialization for single type in parameter pack
template <typename Enum>
struct biggest_underlying_type<Enum>
{
    /*
        Make sure (for safety), that `Enum` type is enumeration, since:
            biggest_underlying_type< NON_ENUM_TYPE >::type
        makes no sense!
    */
    static_assert(std::is_enum<Enum>::value == true, "Enum type must be an enumeration");

    using type = Enum;
};

} // namespace traits
} // namespace variant_enum
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

#include <cstdint> // for std::(u)int{8,16,32,64}_t types

namespace tests {

using namespace ino::meta::variant_enum::traits;

// Unsigned enums
enum class __E_u8  : std::uint8_t  { };
enum class __E_u16 : std::uint16_t { };
enum class __E_u32 : std::uint32_t { };
enum class __E_u64 : std::uint64_t { };

// Signed enums
enum class __E_s8  : std::int8_t  { };
enum class __E_s16 : std::int16_t { };
enum class __E_s32 : std::int32_t { };
enum class __E_s64 : std::int64_t { };

static_assert(std::is_same< biggest_underlying_type<__E_u32, __E_u16, __E_u8>::type, __E_u32 >::value == true, "Test failed");
static_assert(std::is_same< biggest_underlying_type<__E_u8, __E_u16, __E_u32>::type, __E_u32 >::value == true, "Test failed");

static_assert(std::is_same< biggest_underlying_type<__E_u8, __E_u32, __E_u16>::type, __E_u32 >::value == true, "Test failed");
static_assert(std::is_same< biggest_underlying_type<__E_u8, __E_u16, __E_u32, __E_u64>::type, __E_u64 >::value == true, "Test failed");

// Ordering check (make sure, that between DIFFERENT types with SAME sizeof's, being picked first)
static_assert(std::is_same< biggest_underlying_type<__E_u8, __E_s8>::type, __E_u8 >::value == true, "Test failed");
static_assert(std::is_same< biggest_underlying_type<__E_s8, __E_u8>::type, __E_s8 >::value == true, "Test failed");

// Check for working with single type
static_assert(std::is_same< biggest_underlying_type<__E_u32>::type, __E_u32 >::value == true, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
