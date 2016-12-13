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

#include <scraps/log/LoggerInterface.h>

#include <scraps/TaskThread.h>
#include <scraps/TimeValueSamples.h>

#include <chrono>
#include <map>
#include <string>

namespace scraps::log {

/**
 * Rate-limits total output of input messages. If the threshold has been reached,
 * instead of outputing new messages, the logger will go silent (periodically
 * emitting a message that it has gone silent) until the rate of input messages
 * is back under the desired threshold.
 */
class RateLimitedLogger : public LoggerInterface {
public:
    explicit RateLimitedLogger(
        std::shared_ptr<LoggerInterface> destination,
        double maximumMessagesPerSecond,
        std::chrono::system_clock::duration sampleDuration = 15s,
        std::chrono::steady_clock::duration reminderInterval = 5s
    );

    virtual void log(Message message) override;

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

    std::map<std::pair</* file */ std::string, /* line */ unsigned int>, LogMessageState> _messageState;

    std::mutex _mutex;
    std::shared_ptr<LoggerInterface> _destination;
    double _maximumMessagesPerSecond;
    std::chrono::system_clock::duration _sampleDuration;
    std::chrono::steady_clock::duration _reminderInterval;

    TaskThread _reminderThread{"RateLimitedLogger Status"};
};

} // namespace scraps::log
