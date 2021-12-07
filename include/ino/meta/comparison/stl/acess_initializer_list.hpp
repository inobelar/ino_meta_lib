#pragma once

#include <ino/meta/comparison/equal_trait.hpp>

#include <array>

namespace ino {
namespace meta {
namespace comparison {

// Specialization for getting data from std::initializer_list
template <typename T>
struct item_accessor_trait< std::initializer_list<T> >
{
    using collection_t = std::initializer_list<T>;
    using item_type_t  = T;

    static constexpr std::size_t size(const collection_t& list) {
        return list.size();
    }

    static constexpr const item_type_t& item_at(const collection_t& list, std::size_t idx) {
        return *(list.begin() + idx);
    }

};

} // namespace comparison
} // namespace meta
} // namespace ino


