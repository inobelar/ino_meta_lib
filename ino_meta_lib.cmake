#[[
    # Example of usage

    Add into your CMakeLists.txt the next lines:

    # --------------------------------------------------------------------------

    # Include 'ino_meta_lib.cmake' file - to be able to call 'load_ino_meta_lib()'
    include(/path/to/ino_meta_lib/ino_meta_lib.cmake)

    set(INO_META_LIB_INCLUDE_DIRECTORIES "")
    set(INO_META_LIB_HEADERS             "")
    set(INO_META_LIB_SOURCES             "")
    load_ino_meta_lib(INO_META_LIB_INCLUDE_DIRECTORIES INO_META_LIB_HEADERS INO_META_LIB_SOURCES)

    add_executable( <YOUR_EXECUTABLE>
        # ...
        ${INO_META_LIB_HEADERS} ${INO_META_LIB_SOURCES}
    )

    # Add 'ino_meta_lib' includes into search pathes
    target_include_directories( <YOUR_EXECUTABLE> ${INO_META_LIB_INCLUDE_DIRECTORIES} )

    # --------------------------------------------------------------------------
]]

# block(), endblock(), return() added in version 3.25
cmake_minimum_required(VERSION 3.25)

function(load_ino_meta_lib
         include_directories_out headers_out sources_out)

    block(SCOPE_FOR VARIABLES)

        set(INO_META_LIB_INCLUDE_DIRECTORIES
            ${CMAKE_CURRENT_LIST_DIR}/include/
        )

        set(INO_META_LIB_HEADERS
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/SimplePacket/SimplePacket.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/SimplePacket/traits/bytes_offset.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/SimplePacket/traits/count_bytes.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/VariantEnum/VariantEnum.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/VariantEnum/is_same.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/VariantEnum/traits/biggest_underlying_type.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/VariantEnum/traits/get_uint_type.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/VariantEnum/traits/is_all_enums.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/VariantEnum/visit.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/array_utils/make_filled.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/array_utils/reverse.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/array_utils/set_at.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/array_utils/sub_array.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/comparison/equal_trait.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/comparison/stl/access_array.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/comparison/stl/acess_initializer_list.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/comparison/stl/all.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/traits/conjunction.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/traits/contains.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/traits/disjunction.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/traits/function_traits.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/traits/index_of.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/traits/is_all_same.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/traits/is_unique.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/traits/type_at.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/traits/unique.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/typelist/List.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/typelist/convert_tuple.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/typelist/first_types.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/typelist/flatten.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/typelist/join.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/typelist/repeat.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/utils/accumulate.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/utils/index_sequence.hpp
            ${CMAKE_CURRENT_LIST_DIR}/include/ino/meta/utils/index_sequence_reversed.hpp
        )

        set(INO_META_LIB_SOURCES
            "" # Currently no source files
        )

        list(APPEND ${include_directories_out} ${INO_META_LIB_INCLUDE_DIRECTORIES})
        list(APPEND ${headers_out}             ${INO_META_LIB_HEADERS})
        list(APPEND ${sources_out}             ${INO_META_LIB_SOURCES})

        return(PROPAGATE ${include_directories_out} ${headers_out} ${sources_out})

    endblock()

endfunction()
