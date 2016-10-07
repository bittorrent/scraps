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

#include <string>
#include <cstdio>
#include <cinttypes>

// linux defines major and minor macros
#undef major
#undef minor

namespace scraps {

struct Version {
    uint8_t  major    = 0;
    uint8_t  minor    = 0;
    uint16_t revision = 0;
    uint32_t build    = 0;

    static constexpr Version FromInt64(uint64_t i) noexcept;
    static Version FromString(const char* s) noexcept;

    constexpr uint64_t toInt64() const noexcept;
    std::string toString() const;
    std::string toFullString() const;

    constexpr bool operator==(const Version& other) const noexcept { return toInt64() == other.toInt64(); }
    constexpr bool operator!=(const Version& other) const noexcept { return toInt64() != other.toInt64(); }
    constexpr bool operator< (const Version& other) const noexcept { return toInt64() <  other.toInt64(); }
    constexpr bool operator> (const Version& other) const noexcept { return toInt64() >  other.toInt64(); }
    constexpr bool operator<=(const Version& other) const noexcept { return toInt64() <= other.toInt64(); }
    constexpr bool operator>=(const Version& other) const noexcept { return toInt64() >= other.toInt64(); }
};

constexpr Version Version::FromInt64(uint64_t i) noexcept {
    return { uint8_t((i >>  56) & 0x00000000000000ff),
             uint8_t((i >>  48) & 0x00000000000000ff),
             uint16_t((i >> 32) & 0x000000000000ffff),
             uint32_t((i >>  0) & 0x00000000ffffffff)
    };
}

inline Version Version::FromString(const char* s) noexcept {
    Version v;
    std::sscanf(s, "%" SCNu8 ".%" SCNu8 ".%" SCNu16 ".%" SCNu32,
               &v.major,
               &v.minor,
               &v.revision,
               &v.build);
    return v;
}

constexpr uint64_t Version::toInt64() const noexcept {
    return (uint64_t{major}    << 56) +
           (uint64_t{minor}    << 48) +
           (uint64_t{revision} << 32) +
           build;
}

inline std::string Version::toString() const {
    auto s = std::to_string(major) + '.' + std::to_string(minor);
    if (build || revision) { s += '.' + std::to_string(+revision); }
    if (build)             { s += '.' + std::to_string(+build); }
    return s;
}

inline std::string Version::toFullString() const {
    return std::to_string(major)     + '.' +
           std::to_string(minor)     + '.' +
           std::to_string(+revision) + '.' +
           std::to_string(+build);
}

} // namespace scraps
