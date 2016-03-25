#pragma once

#include "scraps/config.h"

#include "scraps/version-config.h"

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

    constexpr Version(uint8_t major, uint8_t minor, uint16_t revision, uint32_t build)
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

static const constexpr Version kVersion = {
#if defined(SCRAPS_VERSION)
    SCRAPS_VERSION_MAJOR,
    SCRAPS_VERSION_MINOR,
    SCRAPS_VERSION_REVISION,
    SCRAPS_VERSION_BUILD
#else
    0, 0, 0, 0
#endif
};

} // namespace scraps
