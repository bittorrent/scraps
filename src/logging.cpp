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
#include <scraps/logging.h>

#include <scraps/filesystem.h>
#include <scraps/loggers.h>

#include <cassert>

#if SCRAPS_APPLE
#import <Foundation/Foundation.h>
#endif

namespace scraps {

std::shared_ptr<Logger> _gLogger;

std::atomic<LogLevel> _gLogLevel{LogLevel::kInfo};

std::shared_ptr<Logger> CreateDefaultFileLogger(const std::string& appName, size_t rotateSize, size_t maxFiles) {
#if SCRAPS_APPLE || SCRAPS_LINUX || SCRAPS_WINDOWS
    assert(!appName.empty());
    std::string logPath = FileLogger::DefaultLogPath(appName);
    CreateDirectory(logPath, true);
    if (IsDirectory(logPath)) {
        if (logPath.back() != '/') {
            logPath += '/';
        }
        logPath += appName + ".log";
        return std::make_shared<FileLogger>(logPath.c_str(), rotateSize, maxFiles);
    }
#endif
    return nullptr;
}

} // namespace scraps
