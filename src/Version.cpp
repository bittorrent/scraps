#include "scraps/Version.h"

namespace scraps {

Version::Version(uint64_t versionInt) {
    build = versionInt & 0x00000000ffffffff;
    versionInt >>= sizeof(build) * 8;
    revision = versionInt & 0x000000000000ffff;
    versionInt >>= sizeof(revision) * 8;
    minor = versionInt & 0x00000000000000ff;
    versionInt >>= sizeof(minor) * 8;
    major = versionInt & 0x00000000000000ff;
}

Version::Version(const std::string& versionStr) {
    size_t start = 0, last = versionStr.find('.');

    major = atoi(versionStr.substr(start, last == std::string::npos ? last : last - start).c_str());
    if (last == std::string::npos) {
        return;
    }
    start = last + 1;
    last  = versionStr.find('.', start);

    minor = atoi(versionStr.substr(start, last == std::string::npos ? last : last - start).c_str());
    if (last == std::string::npos) {
        return;
    }
    start = last + 1;
    last  = versionStr.find('.', start);

    revision = atoi(versionStr.substr(start, last == std::string::npos ? last : last - start).c_str());
    if (last == std::string::npos) {
        return;
    }
    start    = last + 1;

    if (start >= versionStr.size()) {
        return;
    }
    build = atoi(versionStr.substr(start).c_str());
}

uint64_t Version::toInteger() const {
    uint64_t result = major;
    result <<= (sizeof(minor) * 8);
    result += minor;
    result <<= (sizeof(revision) * 8);
    result += revision;
    result <<= (sizeof(build) * 8);
    result += build;
    return result;
}

std::string Version::toString() const {
    std::string result;
    result += std::to_string(major) + "." + std::to_string(minor);
    if (revision || build) {
        result += "." + std::to_string(revision);
    }
    if (build) {
        result += "." + std::to_string(build);
    }
    return result;
}

} // namespace scraps
