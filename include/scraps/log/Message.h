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

#include <chrono>
#include <string>

namespace scraps::log {

/**
* Level values are ordered according to their severity.
*/
enum class Level {
    kDebug,
    kInfo,
    kWarning,
    kError,
};

constexpr const char* LevelString(Level level) {
    switch (level) {
        case Level::kDebug:   return "DEBUG";
        case Level::kInfo:    return "INFO";
        case Level::kWarning: return "WARNING";
        case Level::kError:   return "ERROR";
    }
    return "???";
}

struct Message {
    Level level;
    const char* file;
    unsigned int line;
    std::string text;
    std::chrono::system_clock::time_point time = std::chrono::system_clock::now();
};


} // namespace scraps::log
