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

#include <iterator>
#include <initializer_list>

namespace scraps {
namespace detail {

template <typename Iterable>
class ReverseIterable {
public:
    template <typename T>
    explicit ReverseIterable(T&& iterable) : _iterable(std::forward<T>(iterable)) {}
    auto begin() const { return std::rbegin(_iterable); }
    auto begin()       { return std::rbegin(_iterable); }
    auto   end() const { return std::rend(_iterable); }
    auto   end()       { return std::rend(_iterable); }

private:
    Iterable _iterable;
};

} // namespace detail

/**
 * Used to provide a means to use range for loops in reverse:
 *
 * for (auto& element : Reverse(container)) {
 *    ...
 * }
 */
template <typename Iterable>
auto Reverse(Iterable&& iterable) {
    return detail::ReverseIterable<Iterable>{std::forward<Iterable>(iterable)};
}

template <typename T>
auto Reverse(std::initializer_list<T> iterable) {
    return detail::ReverseIterable<std::initializer_list<T>>{std::move(iterable)};
}

} // namespace scraps
