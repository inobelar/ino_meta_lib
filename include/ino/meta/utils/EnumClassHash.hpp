#pragma once

#include <type_traits> // for std::underlying_type<T>, std::is_enum<T>
#include <functional>  // for std::hash<T>

namespace ino {
namespace meta {
namespace utils {

/**
    @brief Specially designed hasher for creating std::unordered_map with
    enum class as key.

    Simple enums (non-class) works out-the-box in std::unordered_map and dont
    need to specify that hasher.

    ----------------------------------------------------------------------------

    # Example of usage

    @code{.cpp}
        enum class MyEnum {};

        std::unordered_map<MyEnum, int, EnumClassHash> myMap;
    @endcode

    ----------------------------------------------------------------------------

    Mix of both:
        - https://stackoverflow.com/a/24847480
        - https://stackoverflow.com/a/18838036
*/

struct EnumClassHash
{
    template <
            typename EnumT,
  
            // Deduced types from 'EnumT'
            typename underlying_t = typename std::underlying_type<EnumT>::type,
            typename hash_t       = typename std::hash<underlying_t>,
            typename result_t     = typename hash_t::result_type
    >
    result_t operator () (const EnumT& arg) const
    {
        static_assert(std::is_enum<EnumT>::value == true, "EnumT must be an enum");

        // Resulting hash is same, as it being calculated from 'raw integral' value
        hash_t hasher;
        return hasher( static_cast<underlying_t>( arg ) );
    }
};
  
} // namespace utils
} // namespace meta
} // namespace ino
