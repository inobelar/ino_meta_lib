#pragma once

#include <limits> // for std::numeric_limits<T>::max()

#include <ino/meta/traits/is_unique.hpp>
#include <ino/meta/traits/contains.hpp>
#include <ino/meta/traits/index_of.hpp>

#include <ino/meta/VariantEnum/traits/is_all_enums.hpp>
#include <ino/meta/VariantEnum/traits/get_uint_type.hpp>
#include <ino/meta/VariantEnum/traits/biggest_underlying_type.hpp>

namespace ino {
namespace meta {
namespace variant_enum {
/**
    # Example of usage

    @code{.cpp}
    using VarEnum = VariantEnum<
        Fruits,
        Colors,
        Characters
    >;

    VarEnum value(Colors::Blue);

    switch (value.type_index()) {
    case VarEnum::index_of<Fruits>(): {
        std::cout << "Fruit: " << value.get<Fruits>() << std::endl;
    } break;

    case VarEnum::index_of<Colors>(): {
        std::cout << "Color: " << value.get<Colors>() << std::endl;
    } break;

    case VarEnum::index_of<Characters>(): {
        std::cout << "Character: " << value.get<Character>() << std::endl;
    } break;

    default: {
        std::cout << "Undefined" << std::endl;
    } break;
    }
    @endcode
*/
template <typename ... Enums>
class VariantEnum
{
    // Make sure, that each of Enums type is enum
    static_assert(traits::is_all_enums<Enums...>::value == true, "All Enums types must be enums");

    // Make sure, that every Enums types are unique
    static_assert(ino::meta::traits::is_unique<Enums...>::value == true, "All Enums types must be unique");

    // TODO: make sure, that every sizeof( std::underlying_type<Enum>() ) <= std::size_t(type_idx)


public:

    static constexpr std::size_t enums_count = sizeof...(Enums);
    static constexpr std::size_t npos        = (enums_count + 1); // Undefined type idx


    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    using type_idx_t = typename traits::get_uint_type<npos>::type; // Or simply choose std::size_t

    /*
        Make sure, that 'types index' can store enough information.

        For example, if we creates extremely huge (but still possible) variant:
            VarinatEnum< Enums... up to 260 >
        `_type_idx` type must be greater then std::uint8_t (since it's maximum
        value is 255, but we need 261 (260 type indexes + 1 for undefined state),
     */
    static_assert(std::numeric_limits<type_idx_t>::max() >= npos, "Type index cannot describe 'undefined' state");

    // Extra paranoidal check, that `type_idx` is unsigned integral type (since 'signed part' unusable)
    static_assert(std::is_unsigned<type_idx_t>::value == true, "Type index must be an unsigned integral type");

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    /*
        Dynamically deduced (integer) type, based on passed Enums types, and

        ... TODO: DESCRIBE ...

        TODO: describe signed/unsigned enums combination safety

        Reason - to not pick unnessary large type, like `std::uint64_t` for any
        case, for economy.
     */
    using raw_value_t = typename std::underlying_type<
        typename traits::biggest_underlying_type<Enums...>::type
    >::type; // std::uint64_t;

private:

    type_idx_t _type_idx = npos; // Undefined by default

    raw_value_t _value; // Undefined by default

public:

    /// Constructs 'undefined' value
    constexpr VariantEnum()
        : _type_idx( npos ) // Undefined by default
        , _value()
    {}

    /**
        @brief Constructs some 'defined' value

        @note Notice, that this c-tor not marked `explicit`, what allow us to write
        code in two equivalent ways:
        @code{.cpp}
            VarEnum<Colors, Fruits> value(Color::red);
            VarEnum<Colors, Fruits> value = Colors::red;
        @endcode
    */
    template <typename Enum>
    constexpr VariantEnum(const Enum value)
        : _type_idx( index_of<Enum>() )
        , _value( static_cast< raw_value_t >(value) )
    {
        static_assert(is_type_allowed<Enum>() == true, "Unsupported enum type");
    }

    // -------------------------------------------------------------------------
    // Explicetely named c-tors

    static constexpr VariantEnum undefined() {
        return VariantEnum();
    }

    template <typename Enum>
    static constexpr VariantEnum fromValue(const Enum value) {
        return VariantEnum(value);
    }

    // -------------------------------------------------------------------------

    template <typename Enum>
    static constexpr bool is_type_allowed() noexcept
    {
        static_assert(std::is_enum<Enum>::value == true, "Enum must be an enumeration type");

        return ino::meta::traits::contains<Enum, Enums...>::value;
    }

    /// Returns index of enum type in that VariadicEnum
    template <typename Enum>
    static constexpr type_idx_t index_of() noexcept
    {
        static_assert(is_type_allowed<Enum>() == true, "Unsupported enum type");

        return ino::meta::traits::index_of<Enum, Enums...>::value;
    }

    // -------------------------------------------------------------------------

    /// Returns status - is value of some specific type (not undefined)
    template <typename Enum>
    constexpr bool is_type_of() const noexcept
    {
        static_assert(is_type_allowed<Enum>() == true, "Unsupported enum type");

        return ( _type_idx == index_of<Enum>() );
    }

    /// Returns status - is class contains any value
    constexpr bool is_defined() const noexcept
    {
        return (_type_idx < npos);
    }

    /// Returns status - is object NOT contains any value
    constexpr bool is_undefined() const noexcept
    {
        return !is_defined();
    }

    // -------------------------------------------------------------------------

    template <typename Enum>
    constexpr Enum get() const noexcept
    {
        static_assert(is_type_allowed<Enum>() == true, "Unsupported enum type");

        return static_cast< Enum >(_value);
    }

    template <typename Enum>
    inline VariantEnum& set(const Enum value) noexcept
    {
        static_assert(is_type_allowed<Enum>() == true, "Unsupported enum type");

        _type_idx = index_of<Enum>();
        _value = static_cast<raw_value_t>(value);

        return (*this);
    }

    constexpr type_idx_t type_index() const noexcept {
        return _type_idx;
    }


    template <typename Enum>
    inline VariantEnum& operator = (const Enum value) noexcept {
        return this->set(value);
    }

    // -------------------------------------------------------------------------
    // Strict equality comparison check

    constexpr bool operator == (const VariantEnum& other) const noexcept
    {
        return (_type_idx == other._type_idx) &&
               (_value    == other._value);
    }

    constexpr bool operator != (const VariantEnum& other) const noexcept
    {
        return !(*this == other);
    }

};

} // namespace variant_enum
} // namespace meta
} // namespace ino





// -----------------------------------------------------------------------------

#if defined (INO_ENABLE_COMPILETIME_TESTS)
#pragma message "INO_ENABLE_COMPILETIME_TESTS: enabled in " __FILE__

namespace tests {

using namespace ino::meta::variant_enum;

enum class Fruits {
    Orange = 0,
    Apple,
    Kiwi
};

enum class Colors {
    Red = 0,
    Green,
    Blue,
    Magenta,
    Cyan
};

enum class Characters {
    Neo = 0,
    Morpheus,
    Trinity,
    Smith,
    Oracle
};

enum class Unsupported {
    Some = 0,
    Unused,
    Values
};

using VarEnum = VariantEnum<
    Fruits,
    Colors,
    Characters
>;

static_assert( VarEnum().is_undefined() == true , "By default VariantEnum must be 'undefined'");
static_assert( VarEnum().is_defined()   == false, "By default VariantEnum must be 'undefined'");

static_assert( VarEnum(Fruits::Apple).is_defined()   == true,  "Initialized by value - must be 'defined'");
static_assert( VarEnum(Fruits::Apple).is_undefined() == false, "Initialized by value - must be 'defined'");

static_assert( VarEnum(Fruits::Apple).is_defined()   == true,  "Initialized by value - must be 'defined'");
static_assert( VarEnum(Fruits::Apple).is_undefined() == false, "Initialized by value - must be 'defined'");


static_assert( VarEnum::is_type_allowed<Fruits    >()  == true,  "Enums checking works correctly");
static_assert( VarEnum::is_type_allowed<Colors    >()  == true,  "Enums checking works correctly");
static_assert( VarEnum::is_type_allowed<Characters>()  == true,  "Enums checking works correctly");
static_assert( VarEnum::is_type_allowed<Unsupported>() == false, "Enums checking works correctly");



static_assert( VarEnum::index_of<Fruits>()     == 0, "Enum type index deduced correctly");
static_assert( VarEnum::index_of<Colors>()     == 1, "Enum type index deduced correctly");
static_assert( VarEnum::index_of<Characters>() == 2, "Enum type index deduced correctly");

static_assert( VarEnum(Fruits::Apple  ).type_index() == VarEnum::index_of<Fruits>(),     "Enum type index deduced correctly");
static_assert( VarEnum(Colors::Magenta).type_index() == VarEnum::index_of<Colors>(),     "Enum type index deduced correctly");
static_assert( VarEnum(Characters::Neo).type_index() == VarEnum::index_of<Characters>(), "Enum type index deduced correctly");



static_assert( VarEnum(Fruits::Apple  ).is_type_of<Fruits>()     == true , "Value enum type stored correctly");
static_assert( VarEnum(Colors::Magenta).is_type_of<Colors>()     == true , "Value enum type stored correctly");
static_assert( VarEnum(Colors::Green  ).is_type_of<Characters>() == false, "Value enum type stored correctly");



static_assert( VarEnum().get<Colors>() == Colors::Red, "FFUUUU");
static_assert( VarEnum(Colors::Cyan).get<Colors>() == Colors::Cyan, "asd");

static_assert( VarEnum() == VarEnum(),                         "Strict comparison of VariantEnums works");
static_assert( VarEnum() != VarEnum(Colors::Cyan),             "Strict comparison of VariantEnums works");
static_assert( VarEnum(Colors::Cyan) == VarEnum(Colors::Cyan), "Strict comparison of VariantEnums works");
static_assert( VarEnum(Colors::Cyan) != VarEnum(Colors::Blue), "Strict comparison of VariantEnums works");
static_assert( VarEnum(Fruits::Kiwi) != VarEnum(Colors::Blue), "Strict comparison of VariantEnums works");


static_assert( VariantEnum<Colors>().is_undefined() == true, "Variant of single enum type allowed");

// =============================================================================

enum class Enum_u8 : std::uint8_t {
    MAX = std::numeric_limits<std::uint8_t>::max(),
    MIN = std::numeric_limits<std::uint8_t>::min()
};

enum class Enum_s8 : std::int8_t {
    MAX = std::numeric_limits<std::int8_t>::max(),
    MIN = std::numeric_limits<std::int8_t>::min()
};


enum class Enum_u16 : std::uint16_t {
    MAX = std::numeric_limits<std::uint16_t>::max(),
    MIN = std::numeric_limits<std::uint16_t>::min()
};

enum class Enum_s16 : std::int16_t {
    MAX = std::numeric_limits<std::int16_t>::max(),
    MIN = std::numeric_limits<std::int16_t>::min()
};


enum class Enum_u32 : std::uint32_t {
    MAX = std::numeric_limits<std::uint32_t>::max(),
    MIN = std::numeric_limits<std::uint32_t>::min()
};

enum class Enum_s32 : std::int32_t {
    MAX = std::numeric_limits<std::int32_t>::max(),
    MIN = std::numeric_limits<std::int32_t>::min()
};


enum class Enum_u64 : std::uint64_t {
    MAX = std::numeric_limits<std::uint64_t>::max(),
    MIN = std::numeric_limits<std::uint64_t>::min()
};

enum class Enum_s64 : std::int64_t {
    MAX = std::numeric_limits<std::int64_t>::max(),
    MIN = std::numeric_limits<std::int64_t>::min()
};

namespace type_idx_check {

    static_assert(std::is_same< VariantEnum<Enum_u8>::type_idx_t, std::uint8_t >::value == true, "Test failed");
    static_assert(std::is_same< VariantEnum<Enum_u8, Enum_u16>::type_idx_t, std::uint8_t >::value == true, "Test failed");
    static_assert(std::is_same< VariantEnum<Enum_u8, Enum_u16, Enum_u32>::type_idx_t, std::uint8_t >::value == true, "Test failed");
    // TODO: static_assert(std::is_same< VariantEnum<Enum_u8, Enum_u16, Enum_u32 ... Up To 256>::type_idx_t, std::uint8_t >::value == true, "Test failed");

} // namespace type_idx_check

namespace raw_value_check {

    static_assert(std::is_same< VariantEnum<Enum_u8>::raw_value_t, std::uint8_t>::value == true, "Test failed");
    static_assert(std::is_same< VariantEnum<Enum_u8, Enum_u16>::raw_value_t, std::uint16_t>::value == true, "Test failed");
    static_assert(std::is_same< VariantEnum<Enum_u8, Enum_u16, Enum_u32>::raw_value_t, std::uint32_t>::value == true, "Test failed");

    // If enums different (signed/unsigned), but their sizes are equal, picked first type
    static_assert(std::is_same< VariantEnum<Enum_u8, Enum_s8>::raw_value_t, std::uint8_t>::value == true, "Test failed");
    static_assert(std::is_same< VariantEnum<Enum_s8, Enum_u8>::raw_value_t, std:: int8_t>::value == true, "Test failed");


    // Even if 'raw' type is underlying_type< max(Enum_s8,Enum_u16)==Enum_u16 > == std::uint16_t, -128 saved/restored correctly
    static_assert( VariantEnum<Enum_s8, Enum_u16>(Enum_s8::MIN).get<Enum_s8>() == Enum_s8::MIN, "Test failed");

    static_assert( VariantEnum<Enum_s64, Enum_u64>(Enum_s64::MIN).get<Enum_s64>() == Enum_s64::MIN, "Test failed");
    static_assert( VariantEnum<Enum_u64, Enum_s64>(Enum_u64::MAX).get<Enum_u64>() == Enum_u64::MAX, "Test failed");

} // namespace raw_value_check

} // namespace tests
#endif // INO_ENABLE_COMPILETIME_TESTS
