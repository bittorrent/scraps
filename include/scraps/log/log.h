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

#include <scraps/log/Message.h>

#include <scraps/format.h>

#include <atomic>

namespace scraps::log {

class FormattedLogger;
class FormatterInterface;
class LoggerInterface;

/**
* Create the most appropriate file logger for the system.
*
* Android: creates AndroidLogger.
* iOS/Mac OS X: FileLogger
* Linux: FileLogger
* Windows: FileLogger
*
* For file log locations, see FileLogger::DefaultLogPath.
*/
std::shared_ptr<FormattedLogger> CreateFileLogger(
    const std::string&                  appName,
    std::shared_ptr<FormatterInterface> formatter = nullptr,
    size_t                              rotateSize = 1024 * 1024,
    size_t                              maxFiles = 5
);

/**
* Returns the current logger.
*/
std::shared_ptr<LoggerInterface> CurrentLogger();

/**
* Set the current logger.
*/
void SetLogger(std::shared_ptr<LoggerInterface> logger);

/**
* Returns the current log level.
*/
Level CurrentLogLevel();

/**
* Sets the log level (the minimum level to log).
*/
void SetLogLevel(Level level);

/**
* Formats a log message and sends it to the current logger.
*/
template <typename... Args>
void Log(Level level, const char* file, unsigned int line, const char* format, Args&&... args);

/**
* Formats a log message and sends it to the current logger.
*/
template <typename... Args>
void Logf(Level level, const char* file, unsigned int line, const char* format, Args&&... args);


#define SCRAPS_LOG(LEVEL, ...)   ::scraps::log::Log(LEVEL,                           __FILE__, __LINE__, __VA_ARGS__)
#define SCRAPS_LOGF(LEVEL, ...)  ::scraps::log::Logf(LEVEL,                          __FILE__, __LINE__, __VA_ARGS__)
#define SCRAPS_LOG_DEBUG(...)    ::scraps::log::Log(::scraps::log::Level::kDebug,    __FILE__, __LINE__, __VA_ARGS__)
#define SCRAPS_LOGF_DEBUG(...)   ::scraps::log::Logf(::scraps::log::Level::kDebug,   __FILE__, __LINE__, __VA_ARGS__)
#define SCRAPS_LOG_INFO(...)     ::scraps::log::Log(::scraps::log::Level::kInfo,     __FILE__, __LINE__, __VA_ARGS__)
#define SCRAPS_LOGF_INFO(...)    ::scraps::log::Logf(::scraps::log::Level::kInfo,    __FILE__, __LINE__, __VA_ARGS__)
#define SCRAPS_LOG_WARNING(...)  ::scraps::log::Log(::scraps::log::Level::kWarning,  __FILE__, __LINE__, __VA_ARGS__)
#define SCRAPS_LOGF_WARNING(...) ::scraps::log::Logf(::scraps::log::Level::kWarning, __FILE__, __LINE__, __VA_ARGS__)
#define SCRAPS_LOG_ERROR(...)    ::scraps::log::Log(::scraps::log::Level::kError,    __FILE__, __LINE__, __VA_ARGS__)
#define SCRAPS_LOGF_ERROR(...)   ::scraps::log::Logf(::scraps::log::Level::kError,   __FILE__, __LINE__, __VA_ARGS__)

#define SCRAPS_LOG_RATE_LIMITED(LEVEL, INTERVAL, ...)                                  \
    {                                                                                  \
        if ((LEVEL) >= ::scraps::log::detail::gLevel) {                                \
            static ::std::atomic<::std::chrono::steady_clock::time_point> prev;        \
            auto now = ::std::chrono::steady_clock::now();                             \
            auto last = prev.load(::std::memory_order_acquire);                        \
            if (now - last >= (INTERVAL) && prev.compare_exchange_strong(last, now)) { \
                SCRAPS_LOG(LEVEL, __VA_ARGS__);                                        \
            }                                                                          \
        }                                                                              \
    }

#define SCRAPS_LOGF_RATE_LIMITED(LEVEL, INTERVAL, ...)                                 \
    {                                                                                  \
        if ((LEVEL) >= ::scraps::log::detail::gLevel) {                                \
            static ::std::atomic<::std::chrono::steady_clock::time_point> prev;        \
            auto now = ::std::chrono::steady_clock::now();                             \
            auto last = prev.load(::std::memory_order_acquire);                        \
            if (now - last >= (INTERVAL) && prev.compare_exchange_strong(last, now)) { \
                SCRAPS_LOGF(LEVEL, __VA_ARGS__);                                       \
            }                                                                          \
        }                                                                              \
    }

#define SCRAPS_LOG_RATE_LIMITED_DEBUG(INTERVAL, ...)    SCRAPS_LOG_RATE_LIMITED(::scraps::log::Level::kDebug,    INTERVAL, __VA_ARGS__)
#define SCRAPS_LOGF_RATE_LIMITED_DEBUG(INTERVAL, ...)   SCRAPS_LOGF_RATE_LIMITED(::scraps::log::Level::kDebug,   INTERVAL, __VA_ARGS__)
#define SCRAPS_LOG_RATE_LIMITED_INFO(INTERVAL, ...)     SCRAPS_LOG_RATE_LIMITED(::scraps::log::Level::kInfo,     INTERVAL, __VA_ARGS__)
#define SCRAPS_LOGF_RATE_LIMITED_INFO(INTERVAL, ...)    SCRAPS_LOGF_RATE_LIMITED(::scraps::log::Level::kInfo,    INTERVAL, __VA_ARGS__)
#define SCRAPS_LOG_RATE_LIMITED_WARNING(INTERVAL, ...)  SCRAPS_LOG_RATE_LIMITED(::scraps::log::Level::kWarning,  INTERVAL, __VA_ARGS__)
#define SCRAPS_LOGF_RATE_LIMITED_WARNING(INTERVAL, ...) SCRAPS_LOGF_RATE_LIMITED(::scraps::log::Level::kWarning, INTERVAL, __VA_ARGS__)
#define SCRAPS_LOG_RATE_LIMITED_ERROR(INTERVAL, ...)    SCRAPS_LOG_RATE_LIMITED(::scraps::log::Level::kError,    INTERVAL, __VA_ARGS__)
#define SCRAPS_LOGF_RATE_LIMITED_ERROR(INTERVAL, ...)   SCRAPS_LOGF_RATE_LIMITED(::scraps::log::Level::kError,   INTERVAL, __VA_ARGS__)


namespace detail {

extern std::atomic<Level> gLevel;

void LogImpl(Level level, const char* file, unsigned int line, std::string text);

} // namepsace detail

inline Level CurrentLogLevel() { return detail::gLevel; }
inline void SetLogLevel(Level level) { detail::gLevel = level; }

/**
* Formats a log message and sends it to the current logger.
*/
template <typename... Args>
inline void Log(Level level, const char* file, unsigned int line, const char* format, Args&&... args) {
    if (level < detail::gLevel) { return; }
    detail::LogImpl(level, file, line, std::move(Format(format, std::forward<Args>(args)...)));
}

/**
* Formats a log message and sends it to the current logger.
*/
template <typename... Args>
inline void Logf(Level level, const char* file, unsigned int line, const char* format, Args&&... args) {
    if (level < detail::gLevel) { return; }
    detail::LogImpl(level, file, line, std::move(Formatf(format, std::forward<Args>(args)...)));
}

} // namespace scraps::log
