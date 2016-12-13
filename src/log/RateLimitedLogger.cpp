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
#include <scraps/log/RateLimitedLogger.h>

#include <scraps/AverageRate.h>

namespace scraps::log {

RateLimitedLogger::RateLimitedLogger(
    std::shared_ptr<LoggerInterface> destination,
    double maximumMessagesPerSecond,
    std::chrono::system_clock::duration sampleDuration ,
    std::chrono::steady_clock::duration reminderInterval
)
    : _destination{destination}
    , _maximumMessagesPerSecond{maximumMessagesPerSecond}
    , _sampleDuration{sampleDuration}
    , _reminderInterval{reminderInterval}
{
    _logReminders();
}

void RateLimitedLogger::log(Message message) {
    std::lock_guard<std::mutex> lock(_mutex);

    auto& state = _messageState[{std::string{message.file}, message.line}];
    state.history.setMaxSize(static_cast<size_t>(_maximumMessagesPerSecond * 1.5 * std::chrono::duration_cast<std::chrono::duration<double>>(_sampleDuration).count()));
    state.history.insert(message.time, 1);

    auto now = std::chrono::steady_clock::now();
    auto wasLogging = state.isLogging;
    state.mostRecentMessage = message.text;

    if (_tryToResumeLogging(&state, message.time, message.file, message.line)) {
        _destination->log(message);
    } else {
        ++state.numSuppressed[message.level];
        if (wasLogging) {
            auto destinationMessage = message;
            destinationMessage.level = LogLevel::kWarning;
            destinationMessage.text = "[too many log entries: going quiet]";
            _destination->log(std::move(destinationMessage));
            state.lastRateLimitNotification = now;
        }
    }
}

void RateLimitedLogger::_logReminders() {

    auto now = std::chrono::steady_clock::now();
    auto systemNow = std::chrono::system_clock::now();

    std::lock_guard<std::mutex> lock(_mutex);
    for (auto it = _messageState.begin(); it != _messageState.end();) {
        if (_stateIsStale(it->second)) {
            it = _messageState.erase(it);
            continue;
        }
        ++it;
    }

    for (auto& kv : _messageState) {
        auto& fileLine = kv.first;
        auto& state = kv.second;

        if (state.isLogging || state.numSuppressed.empty()) {
            continue;
        }

        if (_tryToResumeLogging(&state, systemNow, fileLine.first.c_str(), fileLine.second)) {
            continue;
        };

        auto highestSuppressedLogLevel = std::prev(state.numSuppressed.end())->first;

        if (now - state.lastRateLimitNotification > _reminderInterval) {
            Message destinationMessage{
                highestSuppressedLogLevel,
                fileLine.first.c_str(),
                fileLine.second,
                Format("[{}, most recent: {}]", _suppressedLogString(state), state.mostRecentMessage),
                systemNow};
            _destination->log(std::move(destinationMessage));
            state.numSuppressed = {};
            state.lastRateLimitNotification = now;
        }
    }

    _reminderThread.asyncAfter(_reminderInterval, [&]{
        _logReminders();
    });
}

bool RateLimitedLogger::_tryToResumeLogging(LogMessageState* state, std::chrono::system_clock::time_point time, const char* file, unsigned int line) {
    auto wasLogging = state->isLogging;
    state->isLogging = !_stateExceedsMessageRate(*state, time);

    if (state->isLogging && !wasLogging) {
        assert(!state->numSuppressed.empty());
        auto highestSuppressedLogLevel = std::prev(state->numSuppressed.end())->first;
        Message destinationMessage{highestSuppressedLogLevel, file, line, "["s + _suppressedLogString(*state) + "]", time};
        _destination->log(std::move(destinationMessage));
        state->numSuppressed = {};
    }

    return state->isLogging;
}

bool RateLimitedLogger::_stateExceedsMessageRate(const LogMessageState& state, std::chrono::system_clock::time_point endTimePoint) {
    auto rate = AverageRate<std::chrono::seconds>(state.history.samples(), endTimePoint - _sampleDuration);
    return rate ? *rate >= _maximumMessagesPerSecond : false;
}

bool RateLimitedLogger::_stateIsStale(const LogMessageState& state) {
    auto now = std::chrono::system_clock::now();
    auto lastSampleTimePoint = std::prev(state.history.samples().end())->first;
    auto firstSampleTimePoint = state.history.samples().begin()->first;
    return state.numSuppressed.empty() && (state.history.samples().empty() || lastSampleTimePoint + _sampleDuration < now || firstSampleTimePoint > now);
}

std::string RateLimitedLogger::_suppressedLogString(const LogMessageState& state){
    return std::accumulate(state.numSuppressed.begin(), state.numSuppressed.end(), std::string{},
            [](auto& carry, auto& kv) {
                auto str = Format("{} {}", std::to_string(kv.second), LevelString(kv.first));
                return carry.empty() ? str : carry + ", " + str; }
        ) + " messages suppressed";
}

} // namespace scraps::log
