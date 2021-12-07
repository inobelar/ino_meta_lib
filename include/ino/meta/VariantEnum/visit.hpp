#pragma once

#include <ino/meta/VariantEnum/VariantEnum.hpp>

#include <ino/meta/traits/function_traits.hpp>

namespace ino {
namespace meta {
namespace variant_enum {

/**
    # Example of usage

    // TODO: add this to run-time 'tests'

    @code{.cpp}
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

    using VarEnum = ino::meta::variant_enum::VariantEnum<
        Fruits,
        Colors,
        Characters
    >;

    void on_character(const Characters f) {
        std::puts("Current Value is: Characters");
    }

    int main()
    {
        const VarEnum var(Characters::Morpheus);

        const bool defined = ino::meta::variant_enum::visit(
            var,
            [&](Fruits f) { // Lambda with capture as arg
                std::puts("Current value is: Fruits");
            },
            [](const Colors f) { // Lambda without capture as arg
                std::puts("Current value is Colors");
            },
            on_character // Common function as arg
        );

        if(!defined) {
            std::puts("Current value is 'Undefined'");
        }

        return 0;
    }
    @endcode
 */

template <typename ... Enums>
class visitor
{
    /**
        # Visiting implementation strategy decisions description

        The most efficient VariantEnum visiting, written-by-hand is simple and
        straightforward switch-case comparison:

        @code{.cpp}
        using VarEnum = VariantEnum<Enum1, Enum2, ..., EnumN>;
        const VarEnum v_enum = Enum2::Value;

        switch( v_enum.type_index() )
        {
            case VarEnum::index_of<Enum1>(): { process_Enum1( v_enum.get<Enum1>() ); } break;
            case VarEnum::index_of<Enum2>(): { process_Enum2( v_enum.get<Enum2>() ); } break; // <-- Jump into it
            ...
            case VarEnum::index_of<EnumN>(): { process_EnumN( v_enum.get<EnumN>() ); } break;
            default: { process_Undefined(); } break;
        }
        @endcode

        ------------------------------------------------------------------------

        Possible various implementations of this behavior described below
    */

    /**
        # 1) If-sequence (the most simple and IMHO most stupid way):

        @code{.cpp}
        if( v_enum.type_index() == VarEnum::index_of<Enum1>() ) {
            process_Enum1( v_enum.get<Enum1>() );
            return true;
        }

        if( v_enum.type_index() == VarEnum::index_of<Enum2>() ) {
            process_Enum2( v_enum.get<Enum2>() );
            return true;
        }

        ...

        if( v_enum.type_index() == VarEnum::index_of<EnumN>() ) {
            process_EnumN( v_enum.get<EnumN>() );
            return true;
        }

        process_Undefined();
        return false;
        @endcode

        may be achieved by
    */

    /**

      # 2) If-else-sequence (currently implemented strategy):

        @code{.cpp}
        if( v_enum.type_index() == VarEnum::index_of<Enum1>() ) {
            process_Enum1( v_enum.get<Enum1>() );
            return true;
        }
        else if( v_enum.type_index() == VarEnum::index_of<Enum2>() ) {
            process_Enum2( v_enum.get<Enum2>() );
            return true;
        }

        ...

        else if( v_enum.type_index() == VarEnum::index_of<EnumN>() ) {
            process_EnumN( v_enum.get<EnumN>() );
            return true;
        }
        else
        {
            process_Undefined();
            return false;
        }
        @endcode
    */

    /**
        Since we cannot "generate" switch-case's by using templates, we can
        simply create jump-table manually:

        - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        using Enum = VariantEnum<
            Colors,    --> type_idx: 0
            Fruits,    --> type_idx: 1
            Characters --> type_idx: 2
            Movies     --> type_idx: 3
            Modes      --> type_idx: 4
        >

        callbacks = [
            Characters --> idx: 0, type_idx = 2
            Colors     --> idx: 1, type_idx = 0
            Modes      --> idx: 2, type_idx = 4
            Fruits     --> idx: 3, type_idx = 1
            Movies     --> idx: 4, type_idx = 3
        ]

                  indexes = <0, 1, 2, 3, 4>
        callbacks_indexes = [1, 3, 0, 4, 2]

        - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        Transformation: from <type_idx> into <callback_idx>:

        const Enum v_enum = Movies::Matrix;
        const std::size_t type_idx = v_enum.type_index(); // --> 3

        const std::size_t callback_idx = callbacks_indexes[type_idx]; // --> 4
        callback = callbacks[ callback_idx ]; // Callbacks::Movies
        callback( v_enum.get<Callback::ArgEnum>() ); // Callbacks::Movies( Matrix );
        - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        BUT we cannot use access-by-runtime-index for parameter pack, like:

            void call_callback(Callbacks&& ... callbacks)
            {
                const std::size_t type_idx     = v_enum.type_index(); // <-- Runtime known index
                const std::size_t callback_idx = callbacks_indexes[type_idx]
                auto& callback = callbacks[callback_idx]; // <-- We still cannot do such powerful magic
                ...
            }

        even if we unpack 'Callbacks&& ...' into std::tuple<Callbacks&&...>,
        access into it's items can be done only by compiletime-known index
        (by using: std::get<Index>(tuple)).
    */

    using VarEnum = VariantEnum<Enums...>;

    // -------------------------------------------------------------------------

    // Shortcut-trait
    template <typename Func>
    struct func_info
    {
        using func_t = typename std::decay<Func>::type; // for removing '&&'

        static constexpr bool is_single_arg = (ino::meta::traits::function_traits< func_t >::arity == 1);
        using first_arg_type = typename ino::meta::traits::function_traits< func_t >::template argument_type_at<0>;
    };

    // -------------------------------------------------------------------------

    template <typename Func, typename ... Funcs>
    static inline bool
    traverse_callbacks(const VarEnum& v_enum, Func&& callback, Funcs&& ... callbacks)
    {
        using func_arg_t = typename func_info<Func>::first_arg_type;
        constexpr auto func_arg_type_idx = VarEnum::template index_of< func_arg_t >();

        if(v_enum.type_index() == func_arg_type_idx)
        {
            // Call callback with current value & stop recursion
            callback( v_enum.template get<func_arg_t>() );
            return true;
        }
        else
        {
            // Continue recursion
            return traverse_callbacks(v_enum, callbacks ...);
        }
    }

    static inline bool
    traverse_callbacks(const VarEnum& /*v_enum*/ ) noexcept
    {
        return false; // Reached recursion end: do nothing
    }

    // -------------------------------------------------------------------------

public:

    template <typename ... Funcs>
    static inline bool visit(const VarEnum& v_enum, Funcs&& ... callbacks)
    {
        /*
            Compile-time safety-checks:

            1. callbacks_count == enums_count
            2. callbacks[i].params.arity == 1
            3. callbacks[i].params[0].type == enum
            4. callbacks[i].params[0].type == supported_enum
            5. callbacks.params.types must be unique (no duplicates)
        */

        // ---------------------------------------------------------------------

        {
            constexpr std::size_t CALLBACKS_COUNT = sizeof... (Funcs);
            static_assert(CALLBACKS_COUNT == VarEnum::enums_count, "Callbacks count not the same as Enums count");
        }


        // ---------------------------------------------------------------------

        {
            constexpr bool is_all_callbacks_single_type =
                    ino::meta::traits::conjunction<
                        typename std::conditional<
                            func_info<Funcs>::is_single_arg,
                            std::true_type,
                            std::false_type
                        >::type ...
                    >::value;
            static_assert(is_all_callbacks_single_type, "All callbacks must have 'strictly single' parameter");
        }

        // ---------------------------------------------------------------------

        {
            constexpr bool is_all_callbacks_arg_enums =
                    ino::meta::traits::conjunction<
                        typename std::conditional<
                            std::is_enum<
                                typename func_info<Funcs>::first_arg_type
                            >::value,
                            std::true_type,
                            std::false_type
                        >::type ...
                    >::value;
            static_assert(is_all_callbacks_arg_enums, "All callbacks must have 'enumeration' parameter types");
        }

        // ---------------------------------------------------------------------

        {
            constexpr bool is_all_callbacks_allowed =
                    ino::meta::traits::conjunction<
                        typename std::conditional<
                            VarEnum::template is_type_allowed<
                                typename func_info<Funcs>::first_arg_type
                            >(),
                            std::true_type,
                            std::false_type
                        >::type ...
                    >::value;

            static_assert(is_all_callbacks_allowed, "All callbacks must have 'allowed' parameter types");
        }

        // ---------------------------------------------------------------------

        {
            constexpr bool is_all_callbacks_unique =
                    ino::meta::traits::is_unique<
                        typename func_info<Funcs>::first_arg_type ...
                    >::value;

            static_assert (is_all_callbacks_unique, "All callbacks must have 'unique' parameter types");
        }

        // ---------------------------------------------------------------------

        // VariantEnum type_index is out of 'callbacks' array bound - do nothing
        if(v_enum.is_undefined()) {
            return false;
        }

        return traverse_callbacks(v_enum, callbacks ...);

        // ----------------------------------------------
        /*
            // Algo idea in imaginary imprative way :)
        
            for(std::size_t i = 0; i < funcs.size(); ++i)
            {
                if( VarEnum::is_type_allowed< funcs[i].arg_t >() )
                {
                    if( v_enum.type_index() == VarEnum::index_of< funcs[i].arg_t >() )
                    {
                        funcs[i]( v_enum.get< funcs[i].arg_t >() ); // call
                        break; // Dont check others
                    }
                }
            }
         */
    }
};

// -----------------------------------------------------------------------------

template <typename ... Enums, typename ... Funcs>
inline bool visit(const VariantEnum<Enums...>& v_enum, Funcs&& ... funcs)
{
    return visitor<Enums...>::visit(v_enum, funcs...);
}

} // namespace variant_enum
} // namespace meta
} // namespace ino
