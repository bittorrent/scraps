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

#include <functional>

namespace scraps::log {

/**
 * Filter log messages with arbitrary predicate.
 */
class FilterLogger : public LoggerInterface {
public:
    // TODO: change function args to `Message`
    using FilterPredicate = std::function<bool(Level level, std::chrono::system_clock::time_point, const char* file, unsigned int line, const std::string& message)>;

    /**
     * Predicate should return true if the message should be logged, false
     * otherwise.
     */
    explicit FilterLogger(std::shared_ptr<LoggerInterface> destination, FilterPredicate predicate)
        : _destination{destination}
        , _predicate{predicate}
    {}

    virtual void log(Message message) override;

private:
    std::shared_ptr<LoggerInterface> _destination;
    FilterPredicate _predicate;
};

} // namespace scraps::log
