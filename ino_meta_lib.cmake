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

# Store 'CMAKE_CURRENT_LIST_DIR' out-of-function, so it will contain path to
# directory of **this** file, otherwise (if use 'CMAKE_CURRENT_LIST_DIR' inside
# of function) it will contain path, related to file where function called).
set(__INO_META_LIB_DIR ${CMAKE_CURRENT_LIST_DIR})

function(load_ino_meta_lib
         include_directories_out headers_out sources_out)

    set(INO_META_LIB_INCLUDE_DIRECTORIES
        ${__INO_META_LIB_DIR}/include/
    )

    set(INO_META_LIB_HEADERS
        ${__INO_META_LIB_DIR}/include/ino/meta/SimplePacket/SimplePacket.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/SimplePacket/traits/bytes_offset.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/SimplePacket/traits/count_bytes.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/VariantEnum/VariantEnum.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/VariantEnum/is_same.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/VariantEnum/traits/biggest_underlying_type.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/VariantEnum/traits/get_uint_type.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/VariantEnum/traits/is_all_enums.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/VariantEnum/visit.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/array_utils/make_filled.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/array_utils/reverse.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/array_utils/set_at.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/array_utils/sub_array.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/comparison/equal_trait.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/comparison/stl/access_array.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/comparison/stl/acess_initializer_list.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/comparison/stl/all.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/traits/conjunction.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/traits/contains.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/traits/disjunction.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/traits/function_traits.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/traits/index_of.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/traits/is_all_same.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/traits/is_unique.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/traits/type_at.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/traits/unique.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/typelist/List.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/typelist/convert_tuple.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/typelist/first_types.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/typelist/flatten.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/typelist/join.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/typelist/repeat.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/utils/accumulate.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/utils/index_sequence.hpp
        ${__INO_META_LIB_DIR}/include/ino/meta/utils/index_sequence_reversed.hpp
    )

    set(INO_META_LIB_SOURCES
        "" # Currently no source files
    )

    list(APPEND ${include_directories_out} ${INO_META_LIB_INCLUDE_DIRECTORIES})
    list(APPEND ${headers_out}             ${INO_META_LIB_HEADERS})
    list(APPEND ${sources_out}             ${INO_META_LIB_SOURCES})

    # Return (reference: https://stackoverflow.com/a/53278726/)
    #   Alternate way (requres CMake 3.25):
    #   'return(PROPAGATE ${include_directories_out} ${headers_out} ${sources_out})'
    set(${include_directories_out} "${${include_directories_out}}" PARENT_SCOPE)
    set(${headers_out}             "${${headers_out}}"             PARENT_SCOPE)
    set(${sources_out}             "${${sources_out}}"             PARENT_SCOPE)

endfunction()
