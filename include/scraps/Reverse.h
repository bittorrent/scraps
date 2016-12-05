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

#include "scraps/config.h"

// needed for Reverse on some specializations of rbegin and rend
#include <iterator>
#include <type_traits>

namespace scraps {
namespace detail {

template <typename R, typename = void>
class NonOwningReverseIteratorWrapper {
public:
    explicit NonOwningReverseIteratorWrapper(R& r) : _range(r) {}
    auto begin() const { return std::rbegin(_range); }
    auto begin()       { return std::rbegin(_range); }
    auto   end() const { return std::rend(_range); }
    auto   end()       { return std::rend(_range); }

private:
    R& _range;
};

template <typename R, typename = void>
class OwningReverseIteratorWrapper {
public:
    explicit OwningReverseIteratorWrapper(R&& r) : _range(r) {}
    auto begin() const { return std::rbegin(_range); }
    auto begin()       { return std::rbegin(_range); }
    auto   end() const { return std::rend(_range); }
    auto   end()       { return std::rend(_range); }

private:
    R _range;
};

} // namespace detail

/**
 * Used to provide a means to use range for loops in reverse:
 *
 * for (auto& element : Reverse(container)) {
 *    ...
 * }
 */
template <typename T>
auto Reverse(T&& t) {
    using IterType = std::conditional_t<
        std::is_rvalue_reference<T>::value,
        detail::NonOwningReverseIteratorWrapper<T>,
        detail::OwningReverseIteratorWrapper<T>
    >;
    return IterType{std::forward<T>(t)};
}

} // namespace scraps
