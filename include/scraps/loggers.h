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

#include "scraps/CircularBuffer.h"
#include "scraps/logging.h"
#include "scraps/thread.h"
#include "scraps/utility.h"
#include "scraps/net/UDPSocket.h"
#include "scraps/TimeValueSamples.h"
#include "scraps/TaskThread.h"

#include <mutex>
#include <stdio.h>
#include <queue>
#include <condition_variable>
#include <initializer_list>
#include <map>
#include <utility>
#include <string>

namespace scraps {

/**
* The StandardLogger class logs messages to stdout and stderr.
*/
class StandardLogger : public Logger {
public:
    virtual void log(LogLevel level, const std::string& message) override;
    virtual ~StandardLogger() {}

private:
    std::mutex _mutex;
};

#if __ANDROID__

/**
* The AndroidLogger class logs messages to the Android log.
*/
class AndroidLogger : public Logger {
public:
    virtual void log(LogLevel level, const std::string& message) override;
    virtual ~AndroidLogger() {}

private:
    std::mutex _mutex;
};

#endif // __ANDROID__

/**
* The FileLogger class logs messages to a file.
*/
class FileLogger : public Logger {
public:
    FileLogger(const char* filePath, size_t rotateSize = 0, size_t maxFiles = 5);
    virtual ~FileLogger();

    virtual void log(LogLevel level, const std::string& message) override;

    FileLogger(const FileLogger&) = delete;
    FileLogger& operator=(const FileLogger&) = delete;

    /**
    * Returns the default logging path for the current system.
    *
    * Mac OS X: ~/Library/Logs/<appName>
    * iOS: %app%/tmp
    * Linux: /var/log/<appName>
    * Windows: %appdata%/<appName>
    */
    static std::string DefaultLogPath(const std::string& appName);

private:
    void _rotate();
    std::string _numberString(size_t n);

    size_t _rotateSize;
    std::string _filePath;
    size_t _maxFiles;
    FILE* _file;
    std::mutex _mutex;
};

/**
* The LoggerLogger class logs messages to other loggers.
*/
class LoggerLogger : public Logger {
public:
    template <typename... Loggers>
    explicit LoggerLogger(Loggers&& ... loggers)
        : _loggers{std::initializer_list<std::shared_ptr<Logger>>{std::forward<Loggers>(loggers)...}}
    {}

    virtual ~LoggerLogger() {}

    virtual void log(LogLevel level, std::chrono::system_clock::time_point time, const char* file, unsigned int line, const std::string& message) override;

private:
    std::vector<std::shared_ptr<Logger>> _loggers;
};

/**
* Forwards logs asyncronously.
*/
class AsyncLogger : public Logger {
public:
    explicit AsyncLogger(std::shared_ptr<Logger> logger);
    virtual ~AsyncLogger();

    virtual void log(LogLevel level, const std::string& message) override;

private:
    void _run();

    std::shared_ptr<Logger>                      _logger;
    std::thread                                  _worker;
    std::mutex                                   _mutex;
    std::condition_variable                      _condition;
    bool                                         _shouldReturn;
    std::queue<std::pair<LogLevel, std::string>> _backlog;
};

/**
 * Filter log messages with arbitrary predicate.
 */
class FilterLogger : public Logger {
public:
    using FilterPredicate = std::function<bool(LogLevel level, std::chrono::system_clock::time_point time, const char* file, unsigned int line, const std::string& message)>;

    /**
     * Predicate should return true if the message should be logged, false
     * otherwise.
     */
    explicit FilterLogger(std::shared_ptr<Logger> destination, FilterPredicate predicate)
        : _destination{destination}
        , _predicate{predicate}
    {}

    virtual void log(LogLevel level, std::chrono::system_clock::time_point time, const char* file, unsigned int line, const std::string& message) override;

private:
    std::shared_ptr<Logger> _destination;
    FilterPredicate _predicate;
};

/**
 * Rate-limits total output of input messages. If the threshold has been reached,
 * instead of outputing new messages, the logger will go silent (periodically
 * emitting a message that it has gone silent) until the rate of input messages
 * is back under the desired threshold.
 */
class RateLimitedLogger : public Logger {
public:
    explicit RateLimitedLogger(std::shared_ptr<Logger> destination, double maximumMessagesPerSecond, std::chrono::system_clock::duration sampleDuration = 15s, std::chrono::steady_clock::duration reminderInterval = 5s)
        : _destination{destination}
        , _maximumMessagesPerSecond{maximumMessagesPerSecond}
        , _sampleDuration{sampleDuration}
        , _reminderInterval{reminderInterval}
    {
        _logReminders();
    }

    virtual void log(LogLevel level, std::chrono::system_clock::time_point time, const char* file, unsigned int line, const std::string& message) override;

private:
    struct LogMessageState {
        bool isLogging = true;
        std::map<LogLevel, size_t> numSuppressed{};
        std::chrono::steady_clock::time_point lastRateLimitNotification{};
        TimeValueSamples<size_t, std::chrono::system_clock::time_point> history;
        std::string mostRecentMessage;
    };

    void _logReminders();
    bool _stateIsStale(const LogMessageState& state);
    std::string _suppressedLogString(const LogMessageState& state);
    bool _stateExceedsMessageRate(const LogMessageState& state, std::chrono::system_clock::time_point endTimePoint);
    bool _tryToResumeLogging(LogMessageState* state, std::chrono::system_clock::time_point time, const char* file, unsigned int line);

    std::map<std::pair</* file */ std::string, /* line */ size_t>, LogMessageState> _messageState;

    std::mutex _mutex;
    std::shared_ptr<Logger> _destination;
    double _maximumMessagesPerSecond;
    std::chrono::system_clock::duration _sampleDuration;
    std::chrono::steady_clock::duration _reminderInterval;

    TaskThread _reminderThread{"RateLimitedLogger Status"};
};

/**
* Sends DogStatsD events for log messages.
*/
class DogStatsDLogger : public Logger {
public:
    explicit DogStatsDLogger(net::Endpoint endpoint);

    virtual void log(LogLevel level, std::chrono::system_clock::time_point time, const char* file, unsigned int line, const std::string& message) override;

    /**
     * Common DogStatsDLogger setup.
     */
    template <typename... DogStatsDLoggerArgs>
    static std::shared_ptr<Logger> FilteredRateLimitedLogger(double maximumMessagesPerSecond, std::chrono::system_clock::duration sampleDuration, std::chrono::steady_clock::duration reminderInterval, DogStatsDLoggerArgs&& ... args) {
        return std::make_shared<FilterLogger>(
            std::make_shared<RateLimitedLogger>(
                std::make_shared<DogStatsDLogger>(std::forward<DogStatsDLoggerArgs>(args)...),
                maximumMessagesPerSecond, sampleDuration, reminderInterval),
            [](LogLevel level, auto...) { return level >= LogLevel::kWarning; });
    }

private:
    std::mutex                      _mutex;
    const net::Endpoint             _endpoint;
    std::unique_ptr<net::UDPSocket> _socket;
};

} // namespace scraps
