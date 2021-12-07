#pragma once

#include <ino/meta/comparison/equal_trait.hpp>

#include <array>

namespace ino {
namespace meta {
namespace comparison {

// Specialization for getting data from std::array
template <typename T, std::size_t SIZE>
struct item_accessor_trait< std::array<T, SIZE> >
{
    using collection_t = std::array<T, SIZE>;
    using item_type_t  = T;

    static constexpr std::size_t size(const collection_t& /*collection*/) {
        return SIZE;
    }

    static constexpr const item_type_t& item_at(const collection_t& collection, std::size_t idx) {
        return collection[idx];
    }

};

} // namespace comparison
} // namespace meta
} // namespace ino
