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

#include <atomic>

namespace scraps::log {

/**
* The LogCounter class counts the number of log messages emitted.
*/
class LogCounter : public LoggerInterface {
public:
    uint64_t count(LogLevel level) { return _counter(level); }

    virtual void log(Message message) override {
        ++_counter(message.level);
    }

private:
    std::atomic<uint64_t> _debugCount{0}, _infoCount{0}, _warningCount{0}, _errorCount{0};

    std::atomic<uint64_t>& _counter(LogLevel level) {
        switch (level) {
            case LogLevel::kDebug: return _debugCount;
            case LogLevel::kInfo: return _infoCount;
            case LogLevel::kWarning: return _warningCount;
            case LogLevel::kError: return _errorCount;
        }
    }
};

} // namespace scraps::log
