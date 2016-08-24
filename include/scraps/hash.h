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
