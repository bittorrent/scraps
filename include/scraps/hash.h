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

#include <vector>
#include <random>
#include <chrono>
#include <cstdint>

namespace scraps {

constexpr uint64_t FNV1A64(const char* str, size_t length) {
    uint64_t hash = 0xcbf29ce484222325;
    for (; length > 0; ++str, --length) {
        hash = (hash ^ (*str & 0xff)) * 0x100000001b3ull;
    }
    return hash;
}

inline namespace literals {
    constexpr uint64_t operator "" _fnv1a64(const char* str, size_t length) {
        return FNV1A64(str, length);
    }
}

/**
* Combines hashes. Implementation is effectively that of boost::hash_combine.
*/
template <class T>
void CombineHash(std::size_t& seed, const T& v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

/**
* Hashes a range of elements.
*/
template <typename It>
std::size_t HashRange(It begin, It end) {
    std::size_t seed = 0;
    for (; begin != end; ++begin) {
        CombineHash(seed, *begin);
    }
    return seed;
}

} // namespace scraps
