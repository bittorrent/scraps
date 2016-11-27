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

#include <scraps/hash.h>

#include <utility>

namespace scraps {

template <int Index, typename... T>
struct TupleHasher {
    static void Hash(size_t* hash, const std::tuple<T...>& tuple) {
        scraps::CombineHash(*hash, std::get<Index>(tuple));
        TupleHasher<Index - 1, T...>::Hash(hash, tuple);
    }
};

template <typename... T>
struct TupleHasher<0, T...> {
    static void Hash(size_t* hash, const std::tuple<T...>& tuple) {
        scraps::CombineHash(*hash, std::get<0>(tuple));
    }
};

} // namespace scraps

namespace std {
template<typename... T>
struct hash<std::tuple<T...>> {
    size_t operator()(const std::tuple<T...>& tuple) const {
        size_t hash = 0;
        scraps::TupleHasher<std::tuple_size<std::tuple<T...>>::value - 1, T...>::Hash(&hash, tuple);
        return hash;
    }
};
} // namespace std
