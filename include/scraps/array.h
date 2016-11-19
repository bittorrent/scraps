/**
* Copyright 2016 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#pragma once

#include <scraps/config.h>

#include <scraps/Temp.h>
#include <scraps/type-traits.h>
#include <scraps/utility.h>

SCRAPS_IGNORE_WARNINGS_PUSH
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
        return HashRange(arr.begin(), arr.end());
    }
};

} // namespace scraps

namespace std {
    template<typename T, size_t N>
    struct hash<std::array<T, N>> {
        size_t operator()(const std::array<T, N>& array) const {
            return scraps::ArrayHasher{}(array);
        }
    };
} // namespace std
