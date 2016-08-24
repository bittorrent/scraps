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
#include "scraps/Version.h"

namespace scraps {

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
