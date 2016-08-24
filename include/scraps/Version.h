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
    uint8_t major     = 0;
    uint8_t minor     = 0;
    uint16_t revision = 0;
    uint32_t build    = 0;

    constexpr Version(uint8_t major, uint8_t minor, uint16_t revision = 0, uint32_t build = 0)
        : major(major), minor(minor), revision(revision), build(build) {}

    explicit Version(uint64_t versionInt);
    explicit Version(const std::string& versionStr);

    uint64_t toInteger() const;

    std::string toString() const;

    bool operator==(const Version& other) const { return toInteger() == other.toInteger(); }
    bool operator!=(const Version& other) const { return toInteger() != other.toInteger(); }
    bool operator<(const Version& other) const { return toInteger() < other.toInteger(); }
    bool operator>(const Version& other) const { return toInteger() > other.toInteger(); }
    bool operator<=(const Version& other) const { return toInteger() <= other.toInteger(); }
    bool operator>=(const Version& other) const { return toInteger() >= other.toInteger(); }
};

} // namespace scraps
