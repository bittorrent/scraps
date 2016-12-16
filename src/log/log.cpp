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
#include <scraps/config.h> // Fixes recursive compiler error
#include <scraps/log/log.h>

#include <scraps/log/AndroidLogger.h>
#include <scraps/log/FileLogger.h>
#include <scraps/log/NullLogger.h>

namespace scraps::log {
namespace detail {

std::atomic<Level> gLevel{Level::kInfo};

namespace {
std::shared_ptr<LoggerInterface> gLogger = std::make_shared<NullLogger>();
} // anonymous namespace

void LogImpl(Level level, const char* file, unsigned int line, std::string text) {
    Message message{level, file, line, std::move(text)};
    gLogger->log(std::move(message));
}

} // namespace detail

std::shared_ptr<FormattedLogger> CreateFileLogger(
    const std::string&                  appName,
    std::shared_ptr<FormatterInterface> formatter,
    size_t                              rotateSize,
    size_t                              maxFiles
) {
#if SCRAPS_APPLE || SCRAPS_LINUX || SCRAPS_WINDOWS
    return FileLogger::CreateApplicationLogger(std::move(formatter), appName, rotateSize, maxFiles);
#elif SCRAPS_ANDROID
    return std::make_shared<AndroidLogger>(std::move(formatter));
#else
    return std::make_shared<NullLogger>();
#endif
}

std::shared_ptr<LoggerInterface> CurrentLogger() {
    return detail::gLogger;
}

void SetLogger(std::shared_ptr<LoggerInterface> logger) {
    detail::gLogger = logger ? std::move(logger) : std::make_shared<NullLogger>();
}

} // namespace scraps::log
