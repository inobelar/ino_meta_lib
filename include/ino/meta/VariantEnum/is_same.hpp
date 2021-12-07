#pragma once

#include <ino/meta/VariantEnum/VariantEnum.hpp>

namespace ino {
namespace meta {
namespace variant_enum {

namespace detail {

template <typename ... EnumsA>
struct compare
{
    /*
        Nested template structure, because we cannot declare `compare` struct
        with two different parameter packs, like:

            template<typename ... EnumsA, typename ... EnumsB>
            struct compare { ... };
    */
    template <typename ... EnumsB>
    class with
    {
        // ---------------------------------------------------------------------

        // Convenient aliases
        using VarEnumA = VariantEnum<EnumsA ...>;
        using VarEnumB = VariantEnum<EnumsB ...>;

        template <typename Enum>
        struct is_both_contains_enum
        {
            constexpr static bool value =
                ( VarEnumA::template is_type_allowed<Enum>() == true ) &&
                ( VarEnumB::template is_type_allowed<Enum>() == true );
        };

        // Convenient shortcut
        template <bool Enabled>
        using enable_bool_if = typename std::enable_if< Enabled, bool >::type;

        // ---------------------------------------------------------------------

        /*
            Comparison functions below uses SFINAE to not even try to compile
            non-allowed checks.

            Them all marked as `constexpr`, because comparation really can be
            done in compile-time! Also this is required feature for compile-time
            tests.
        */


        /*
            Comparison function, which compiles (if enabled), only if BOTH of
            VariantEnums contains some `Enum` type.

            Otherwise we cannot call:
                VarEnumA::indexOf<Enum>() or VarEnumB::indexOf<Enum>()
            if for some of them such `Enum` type is not allowed.

            In such case we ensure, that both VariantEnums currently contains
            enums of same type (both type_index'es points into same `Enum` type)
            and if its true - we compare the actual enum values.
        */
        template <
                std::size_t Index, std::size_t Count,

                typename Enum, typename ... Rest
        >
        static constexpr
            enable_bool_if<
                (Index < Count) &&                           // <-- Enabled if we still traversing
                (is_both_contains_enum<Enum>::value == true) // <-- Enabled if we can call both `index_of`
            >
        is_same_impl(const VarEnumA& v_enum_a, const VarEnumB& v_enum_b) noexcept
        {
            // In C++11 we cannot use `if`'s and declare variables in constexpr funcs :C
            // Thats why here is ternary conditional operator.

            return
                    // If (various enums current-types (type_index()) points into same type)
                    (
                        (v_enum_a.type_index() == VarEnumA::template index_of<Enum>()) &&
                        (v_enum_b.type_index() == VarEnumB::template index_of<Enum>())
                    )
                    ?
                    // Then
                    (
                        // Return result - is values (of same type) are equal
                        (v_enum_a.template get<Enum>() == v_enum_b.template get<Enum>())
                    )
                    :
                    // Else
                    (
                        // Continue recursion
                        is_same_impl<Index + 1, Count, Rest ...>(v_enum_a, v_enum_b)
                    );
        }


        /*
            Comparison function, which compiles (if enabled), only if NOT BOTH
            VariantEnums contains some `Enum` type --> if SOME of them NOT
            contains `Enum` type.

            In such case we dont try to compare any values - type-index'es and
            actual enumerations values - simply continue recursion for next
            types comparison.
        */
        template <
                std::size_t Index, std::size_t Count,

                typename Enum, typename ... Rest
        >
        static constexpr
            enable_bool_if<
                (Index < Count) &&                            // <-- Enabled if we still traversing
                (is_both_contains_enum<Enum>::value == false) // <-- Enabled if we cannot call both `index_of`
            >
        is_same_impl(const VarEnumA& v_enum_a, const VarEnumB& v_enum_b) noexcept
        {
            // Continue recursion
            return is_same_impl<Index + 1, Count, Rest ...>(v_enum_a, v_enum_b);
        }


        /*
            Comparison function, which compiles (if enabled), only if we reach
            end-of-recusion --> (Index == Count).

            That means, we compare all cases, but all them was non-successful.
        */
        template <
                std::size_t Index, std::size_t Count,

                typename ... Rest // <-- Empty at that case
        >
        static constexpr
            enable_bool_if<
                (Index == Count) // <-- Enabled if we not traversing
            >
        is_same_impl(const VarEnumA& /*v_enum_a*/, const VarEnumB& /*v_enum_b*/) noexcept
        {
            return false;
        }

    public:

        // ---------------------------------------------------------------------

        // Convenient wrapper
        template <typename ... Enums>
        static constexpr
        bool is_same(const VarEnumA& v_enum_a, const VarEnumB& v_enum_b) noexcept
        {
            return is_same_impl</*Index=*/ 0, /*Count=*/ (sizeof... (Enums)), Enums...>(v_enum_a, v_enum_b);
        }

    };
};

} // namespace detail

/*
    VarEnumA = VariantEnum< A, B, C, D, E >
    VarEnumB = VariantEnum< D, E, F, G, H >

    TODO: extract overlaping types (D, E), and compare only for them
*/

template <typename ... EnumsA, typename ... EnumsB>
constexpr bool is_same(const VariantEnum<EnumsA...>& v_enum_a,
                       const VariantEnum<EnumsB...>& v_enum_b) noexcept
{
    // In C++11 we cannot use `if`'s and declare variables in constexpr funcs :C
    // Thats why here is ternary conditional operator.

    return
            // If
            (
                (v_enum_a.is_defined() && v_enum_b.is_defined())
            )
            ?
            // Then
            (
                // Both A && B are defined --> compare type_index'es & enum values
                detail::compare<EnumsA...>::template with<EnumsB...>::template is_same<EnumsA...>(v_enum_a, v_enum_b)
            )
            :
            // Else (Some of A or B is undefined)
            (
                // Them are 'the same' if both are undefined
                (v_enum_a.is_undefined() == v_enum_b.is_undefined())
            );
}

} // namespace variant_enum
} // namespace meta
} // namespace ino




// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

namespace tests {

using namespace ino::meta;

static_assert( is_same(VariantEnum<Fruits, Colors>(), VariantEnum<Characters>()) == true, "Test failed");

static_assert( is_same(VariantEnum<Fruits, Colors>(Colors::Green), VariantEnum<Colors, Characters>(Colors::Green)) == true, "Test failed");

// TODO: add some additional tests
//using DifferVarEnum = VariantEnum<Unsupported, Characters>;
//static_assert( VarEnum().is_same_value( DifferVarEnum() ) == true, "asd");
//static_assert( VarEnum().is_same_value( DifferVarEnum(Unsupported::Unused) ) == false, "asd");
//static_assert( VarEnum(Fruits::Kiwi).is_same_value( DifferVarEnum() ) == false, "asd");
//static_assert( VarEnum(Fruits::Kiwi).is_same_value( DifferVarEnum(Unsupported::Unused) ) == false, "asd");
//static_assert( VarEnum(Characters::Morpheus).is_same_value( DifferVarEnum(Characters::Neo) ) == false, "asd");
//static_assert( VarEnum(Characters::Neo).is_same_value( DifferVarEnum(Characters::Neo) ) == false, "asd");

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
