#pragma once

#include <type_traits> // for std::coditional
#include <limits>      // for std::numeric_limits<T>::max()
#include <cstdint>     // for std::(u)int{8,16,32,64}_t

namespace ino {
namespace meta {
namespace variant_enum {
namespace traits {

// NOTE: Works only for sorted sequency of types.
// TODO: MUST BE IMPROVED for 'sets' of types, not sequency

/**
    Implementation below may be a lot simplified, but non-flexible:

    @code{.cpp}
    //    if( count <= uint8::max() ) {
    //        type = uint8;
    //    } else if( count <= uint16::max() ) {
    //        type = uint16;
    //    } else if(count <= uint32::max() ) {
    //        type = uint32;
    //    } else {
    //        type = uint64;
    //    }
    template <std::size_t Count>
    struct get_type
    {
        using type = typename std::conditional<
            (Count <= std::numeric_limits<std::uint8_t>::max() ),

            std::uint8_t,

            typename std::conditional<
                (Count <= std::numeric_limits<std::uint16_t>::max()),

                std::uint16_t,

                typename std::conditional<
                    (Count <= std::numeric_limits<std::uint32_t>::max()),

                    std::uint32_t,

                    std::uint64_t // <-- or do recursion here
                >::type
            >::type
        >::type;
    };
    @endcode
*/

template <std::size_t Count, typename ... Types>
struct get_type;

template <std::size_t Count, typename T, typename Head, typename ... Rest>
struct get_type<Count, T, Head, Rest ...>
{
    using type = typename get_type<Count,
        T, typename get_type<Count, Head, Rest ...>::type
    >::type;
};

template <std::size_t Count, typename LowerT, typename GreaterT>
struct get_type<Count, LowerT, GreaterT>
{
    static constexpr auto lower_t_max   = std::numeric_limits<LowerT  >::max();
    static constexpr auto greater_t_max = std::numeric_limits<GreaterT>::max();

    // Additional (possibly unnecessary) check to make sure, that types passed
    // in Ascending Order, because otherwise algorithm will not work correctly
    //
    // Notice, that `<=` here, not `<`, which allow us to relax rules and
    // pass multiple same types without breaking everything:
    //   get_type<Count, std::uint8_t, std::uint16_t, std::uint16_t, std::uint32_t, std::uint64_t>
    static_assert(lower_t_max <= greater_t_max, "LowerT must be smaller, than GreaterT");

    // Make sure, that we can choose GreaterT
    static_assert(Count <= greater_t_max, "GreaterT type cannot represent Count value");

    using type = typename std::conditional<
        (Count <= lower_t_max),
        LowerT,
        GreaterT
    >::type;
};

template <std::size_t Count>
struct get_uint_type
        : get_type<Count, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>
{};

} // namespace traits
} // namespace variant_enum
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

namespace tests {

using namespace ino::meta::variant_enum::traits;

static_assert(std::is_same< get_uint_type<0>::type, std::uint8_t>::value, "Test failed");

static_assert(std::is_same< get_uint_type<255>::type, std::uint8_t>::value, "Test failed");
static_assert(std::is_same< get_uint_type<256>::type, std::uint16_t>::value, "Test failed");

static_assert(std::is_same< get_uint_type<65535>::type, std::uint16_t>::value, "Test failed");
static_assert(std::is_same< get_uint_type<65536>::type, std::uint32_t>::value, "Test failed");

static_assert(std::is_same< get_uint_type<4294967295>::type, std::uint32_t>::value, "Test failed");
static_assert(std::is_same< get_uint_type<4294967296>::type, std::uint64_t>::value, "Test failed");

static_assert(std::is_same< get_uint_type<9223372036854775807>::type, std::uint64_t>::value, "Test failed");
// (9223372036854775807 + 1) - not allowed

// Overflow test
static_assert(std::is_same< get_uint_type< static_cast<std::size_t>(-1) >::type, std::uint64_t>::value, "Test failed");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
