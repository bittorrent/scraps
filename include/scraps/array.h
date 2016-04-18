#pragma once

#include "scraps/config.h"

#include "scraps/Temp.h"
#include "scraps/type-traits.h"

SCRAPS_IGNORE_WARNINGS_PUSH
#include <boost/functional/hash.hpp>

#include <gsl.h>
SCRAPS_IGNORE_WARNINGS_POP

#include <array>

namespace scraps {

/**
 * Convenience functions to create an std::array from a gsl::span
 */
template <typename T, std::ptrdiff_t ArraySize, template <typename, std::ptrdiff_t...> class Span>
constexpr std::array<std::remove_cv_t<T>, ArraySize> ToArray(Span<T, ArraySize> span) {
    static_assert(ArraySize != gsl::dynamic_range, "Dynamic range spans are not allowed.");
    static_assert(ArraySize > 0, "ArraySize must be greater than 0");

    std::array<std::remove_cv_t<T>, ArraySize> ret{};
    std::copy(span.begin(), span.end(), ret.begin());
    return ret;
}

template <typename T, std::ptrdiff_t ArraySize, template <typename, std::ptrdiff_t...> class Span>
constexpr std::array<std::remove_cv_t<T>, ArraySize> ToArray(Temp<Span<T, ArraySize>> span) {
    return ToArray(static_cast<Span<T, ArraySize>>(span));
}

template <typename T, size_t N>
constexpr std::array<std::remove_cv_t<T>, N> ToArray(T (&arr)[N]) {
    std::array<std::remove_cv_t<T>, N> ret{};
    std::copy(arr, arr + N, ret.begin());
    return ret;
}

/**
 * Convenience type for defining a c-style array using the template parameters
 * of a std::array.
 */
template <typename ArrayT>
using CArray = typename RemoveCVRType<ArrayT>::value_type[std::tuple_size<RemoveCVRType<ArrayT>>::value];

/**
 * Hashing struct suitable for using an std::array as the key in stl
 * associative containers such as unordered_map.
 */
struct ArrayHasher {
    template <typename T, size_t N>
    size_t operator()(const std::array<T, N>& arr) const {
        return boost::hash_range(arr.begin(), arr.end());
    }
};

} // namespace scraps
