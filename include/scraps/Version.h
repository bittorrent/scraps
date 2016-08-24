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

// XXX: linux has major/minor macros defined somewhere, so let's nuke them here
#if defined(major)
#undef major
#endif

#if defined(minor)
#undef minor
#endif

namespace scraps {

struct Version {
    uint8_t  major    = 0;
    uint8_t  minor    = 0;
    uint16_t revision = 0;
    uint32_t build    = 0;

    constexpr Version(uint8_t major, uint8_t minor, uint16_t revision = 0, uint32_t build = 0) noexcept;
    constexpr explicit Version(uint64_t versionInt) noexcept;
    explicit Version(const std::string& versionStr);

    constexpr uint64_t toInteger() const noexcept;
    std::string toString() const;

    bool operator==(const Version& other) const { return toInteger() == other.toInteger(); }
    bool operator!=(const Version& other) const { return toInteger() != other.toInteger(); }
    bool operator< (const Version& other) const { return toInteger() <  other.toInteger(); }
    bool operator> (const Version& other) const { return toInteger() >  other.toInteger(); }
    bool operator<=(const Version& other) const { return toInteger() <= other.toInteger(); }
    bool operator>=(const Version& other) const { return toInteger() >= other.toInteger(); }
};

constexpr Version::Version(uint8_t major, uint8_t minor, uint16_t revision, uint32_t build) noexcept
    : major(major)
    , minor(minor)
    , revision(revision)
    , build(build)
{}

constexpr Version::Version(uint64_t versionInt) noexcept
    : major   {uint8_t((versionInt >>  56) & 0x00000000000000ff)}
    , minor   {uint8_t((versionInt >>  48) & 0x00000000000000ff)}
    , revision{uint16_t((versionInt >> 32) & 0x000000000000ffff)}
    , build   {uint32_t((versionInt >>  0) & 0x00000000ffffffff)}
{}

constexpr uint64_t Version::toInteger() const noexcept {
    return (uint64_t{major} << 56) +
           (uint64_t{minor} << 48) +
           (uint64_t{revision} << 32) +
           build;
}

} // namespace scraps
