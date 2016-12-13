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
#include <scraps/log/FilterLogger.h>
#include <scraps/log/RateLimitedLogger.h>

#include <scraps/net/UDPSocket.h>

namespace scraps::log {

/**
* Sends DogStatsD events for log messages.
*/
class DogStatsDLogger : public LoggerInterface {
public:
    explicit DogStatsDLogger(net::Endpoint endpoint);

    virtual void log(Message message) override;

    /**
     * Common DogStatsDLogger setup.
     */
    template <typename... DogStatsDLoggerArgs>
    static std::shared_ptr<LoggerInterface> FilteredRateLimitedLogger(
        double maximumMessagesPerSecond,
        std::chrono::system_clock::duration sampleDuration,
        std::chrono::steady_clock::duration reminderInterval,
        DogStatsDLoggerArgs&& ... args
    ) {
        return std::make_shared<FilterLogger>(
            std::make_shared<RateLimitedLogger>(
                std::make_shared<DogStatsDLogger>(std::forward<DogStatsDLoggerArgs>(args)...),
                maximumMessagesPerSecond, sampleDuration, reminderInterval
            ),
            [](LogLevel level, auto...) { return level >= LogLevel::kWarning; }
        );
    }

private:
    std::mutex                      _mutex;
    const net::Endpoint             _endpoint;
    std::unique_ptr<net::UDPSocket> _socket;
};

} // namespace scraps::log
