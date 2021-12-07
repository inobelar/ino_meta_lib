#pragma once

#include <cstddef> // for std::size_t

namespace ino {
namespace meta {
namespace comparison {

template <typename T>
struct item_accessor_trait;

// Default implementation - needed for comparison, some types with itself, and
// also for comparison any types with collections, like: POD <--> std::array<POD, 1>
template <typename T>
struct item_accessor_trait
{
    using collection_t = T;
    using item_type_t  = T;

    static constexpr std::size_t size(const collection_t& /*collection*/) {
        return 1;
    }

    static constexpr const item_type_t& item_at(const collection_t& value, std::size_t /*idx*/) {
        return value;
    }

};

// Specialization for getting data from raw array
template <typename T, std::size_t SIZE>
struct item_accessor_trait< T[SIZE] >
{
    using collection_t = T[SIZE];
    using item_type_t  = T;

    static constexpr std::size_t size(const collection_t& /*collection*/) {
        return SIZE;
    }

    static constexpr const item_type_t& item_at(const collection_t& collection, std::size_t idx) {
        return collection[idx];
    }

};

template <typename COLLECTION_TYPE_A, typename COLLECTION_TYPE_B>
struct collection_equal_trait
{
    using collection_a_t = COLLECTION_TYPE_A;
    using collection_b_t = COLLECTION_TYPE_B;

    using item_a_t = typename item_accessor_trait<collection_a_t>::item_type_t;
    using item_b_t = typename item_accessor_trait<collection_b_t>::item_type_t;

    // Function pointers for getting collection items
    //
    // Needed for accessing collection items by index, since in various collections
    // it may be in different ways:
    //     collection[i]             : arrays
    //     *(coolection.begin() + i) : initializer_list
    using a_item_getter = item_a_t const& (*) (const collection_a_t& a, std::size_t idx);
    using b_item_getter = item_b_t const& (*) (const collection_b_t& b, std::size_t idx);

    static constexpr a_item_getter get_item_a = item_accessor_trait<collection_a_t>::item_at;
    static constexpr b_item_getter get_item_b = item_accessor_trait<collection_b_t>::item_at;

private:

    static constexpr
        bool items_equal(
            const collection_a_t& a, const collection_b_t& b,
            const std::size_t SIZE_A, const std::size_t SIZE_B,
            std::size_t i = 0)
    {
        // At this point SIZE_A == SIZE_B, so we simply used one of them

        return (i < SIZE_A)
                ?
                (
                    (get_item_a(a, i) == get_item_b(b, i)) &&
                    items_equal(a, b, SIZE_A, SIZE_B, i+1) // Continue recursive comparison
                )
                :
                true; // (i >= SIZE_A) --> Stop recursion
    }

    static constexpr
        bool equal_impl(const collection_a_t& a, const collection_b_t& b,
                        const std::size_t SIZE_A, const std::size_t SIZE_B)
    {
        return ( SIZE_A != SIZE_B )
                ?
                    false // Sizes not equal - comparison not matters
                :
                    items_equal(a, b, SIZE_A, SIZE_B);
    }

public:

    static constexpr
        bool equal(const collection_a_t& a, const collection_b_t& b)
    {
        return equal_impl(a, b,
                          item_accessor_trait<collection_a_t>::size(a),
                          item_accessor_trait<collection_b_t>::size(b)
        );
    }
};

// -----------------------------------------------------------------------------

template <typename T, typename U>
constexpr bool equal(const T& a, const U& b) {
    return collection_equal_trait<T, U>::equal(a, b);
}

} // namespace comparison
} // namespace meta
} // namespace ino
