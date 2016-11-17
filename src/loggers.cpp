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
#include <scraps/AverageRate.h>
#include <scraps/loggers.h>

#if SCRAPS_APPLE
#import <Foundation/Foundation.h>
#elif SCRAPS_ANDROID
#include <android/log.h>
#endif

#include <cmath>
#include <cassert>

namespace scraps {

void StandardLogger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock{_mutex};
#ifdef __ANDROID__
    auto f = stdout; // only stdout for android
#else
    auto f = level <= LogLevel::kInfo ? stdout : stderr;
#endif
    fprintf(f, "%s\n", message.c_str());
    fflush(f);
}

#if __ANDROID__
void AndroidLogger::log(LogLevel level, const std::string& message) {
    __android_log_print(level == LogLevel::kInfo ? ANDROID_LOG_INFO : ANDROID_LOG_ERROR, "live", "%s", message.c_str());
}
#endif

FileLogger::FileLogger(const char* filePath, size_t rotateSize, size_t maxFiles)
    : _rotateSize(rotateSize), _filePath(filePath), _maxFiles(maxFiles) {
    if (!(_file = fopen(filePath, "a+"))) { SCRAPS_LOGF_ERROR("couldn't open %s for logging", _filePath); }
}

FileLogger::~FileLogger() { fclose(_file); }

void FileLogger::log(LogLevel level, const std::string& message) {
    if (!_file) { return; }

    std::lock_guard<std::mutex> lock{_mutex};

    auto pos = ftell(_file);
    if (_rotateSize && (pos == -1 || static_cast<size_t>(pos) >= _rotateSize)) {
        _rotate();
    }

    fprintf(_file, "%s\n", message.c_str());
    fflush(_file);
}

std::string FileLogger::DefaultLogPath(const std::string& appName) {
#if SCRAPS_MACOS
    return [[NSString stringWithFormat:@"%@/Library/Logs/%s/", NSHomeDirectory(), appName.c_str()] UTF8String];
#elif SCRAPS_IOS || SCRAPS_TVOS
    return [NSTemporaryDirectory() UTF8String];
#elif SCRAPS_LINUX
    return "/var/log/" + appName + "/";
#elif SCRAPS_ANDROID
    return "";
#elif SCRAPS_WINDOWS
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path))) {
        return std::string(path) + "\\" + appName + "\\";
    }

    return "";
#endif
}

void FileLogger::_rotate() {
    fclose(_file);
    for (size_t i = _maxFiles - 1; i >= 1; --i) {
        auto path = _filePath + '.' + _numberString(i);
        if (i == _maxFiles - 1) {
            remove(path.c_str());
        } else {
            rename(path.c_str(), (_filePath + '.' + _numberString(i + 1)).c_str());
        }
    }
    rename(_filePath.c_str(), (_filePath + '.' + _numberString(1)).c_str());
    _file = fopen(_filePath.c_str(), "w+");
}

std::string FileLogger::_numberString(size_t n) {
    int precision = std::log10((double)(_maxFiles - 1)) + 1;
    return Formatf(Formatf("%%.%du", precision).c_str(), n);
}

void LoggerLogger::log(LogLevel level, std::chrono::system_clock::time_point time, const char* file, unsigned int line, const std::string& message) {
    for (auto& logger : _loggers) {
        logger->log(level, time, file, line, message);
    }
}

AsyncLogger::AsyncLogger(std::shared_ptr<Logger> logger) : _logger(logger), _shouldReturn(false) {
    _worker = std::thread(&AsyncLogger::_run, this);
}

AsyncLogger::~AsyncLogger() {
    {
        std::lock_guard<std::mutex> lock{_mutex};
        _shouldReturn = true;
    }
    _condition.notify_one();
    if (_worker.joinable()) { _worker.join(); }
}

void AsyncLogger::log(LogLevel level, const std::string& message) {
    {
        std::lock_guard<std::mutex> lock{_mutex};
        _backlog.emplace(level, message);
    }
    _condition.notify_one();
}

void AsyncLogger::_run() {
    SetThreadName("AsyncLogger");

    std::unique_lock<std::mutex> lock{_mutex};
    while (!_shouldReturn) {
        if (_backlog.empty()) { _condition.wait(lock); }
        while (!_backlog.empty()) {
            LogLevel level{};
            std::string message;
            std::tie(level, message) = std::move(_backlog.front());
            _backlog.pop();

            lock.unlock(); // unlock to call out
            _logger->log(level, message);
            lock.lock();
        }
    }
}

void FilterLogger::log(LogLevel level, std::chrono::system_clock::time_point time, const char* file, unsigned int line, const std::string& message) {
    if (_predicate(level, time, file, line, message)) {
        _destination->log(level, time, file, line, message);
    }
}

void RateLimitedLogger::log(LogLevel level, std::chrono::system_clock::time_point time, const char* file, unsigned int line, const std::string& message) {
    std::lock_guard<std::mutex> lock(_mutex);

    auto& state = _messageState[{std::string{file}, line}];
    state.history.setMaxSize(static_cast<size_t>(_maximumMessagesPerSecond * 1.5 * std::chrono::duration_cast<std::chrono::duration<double>>(_sampleDuration).count()));
    state.history.insert(time, 1);

    auto now = std::chrono::steady_clock::now();
    auto wasLogging = state.isLogging;
    state.mostRecentMessage = message;

    if (_tryToResumeLogging(&state, time, file, line)) {
        _destination->log(level, time, file, line, message);
    } else {
        ++state.numSuppressed[level];
        if (wasLogging) {
            _destination->log(LogLevel::kWarning, time, file, line, "[too many log entries: going quiet]");
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
            _destination->log(highestSuppressedLogLevel, systemNow, fileLine.first.c_str(), fileLine.second, Format("[{}, most recent: {}]", _suppressedLogString(state), state.mostRecentMessage));
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
        _destination->log(highestSuppressedLogLevel, time, file, line, "["s + _suppressedLogString(*state) + "]");
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
                auto str = Format("{} {}", std::to_string(kv.second), LogLevelString(kv.first));
                return carry.empty() ? str : carry + ", " + str; }
        ) + " messages suppressed";
}

DogStatsDLogger::DogStatsDLogger(net::Endpoint endpoint)
    : _endpoint{std::move(endpoint)}
    , _socket{std::make_unique<net::UDPSocket>(net::UDPSocket::Protocol::kIPv4)}
{}

void DogStatsDLogger::log(LogLevel level, std::chrono::system_clock::time_point time, const char* file, unsigned int line, const std::string& message) {
    std::lock_guard<std::mutex> lock{_mutex};

    auto text = Format("{} ({}:{})", message, file, line);
    auto data = Format("_e{{{},{}}}:{}|{}|t:{}\n", message.size(), text.size(), message, text,
        level == LogLevel::kError ? "error" : (level == LogLevel::kWarning ? "warning" : "info")
    );

    _socket->send(_endpoint, data.data(), data.size());
}

} // namespace scraps
