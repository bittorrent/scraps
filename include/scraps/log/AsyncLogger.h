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

#include <mutex>
#include <queue>
#include <condition_variable>

namespace scraps::log {

/**
* Forwards logs asyncronously.
*/
class AsyncLogger : public LoggerInterface {
public:
    explicit AsyncLogger(std::shared_ptr<Logger> logger);
    virtual ~AsyncLogger();

    virtual void log(Message message) override;

private:
    void _run();

    std::shared_ptr<LoggerInterface> _logger;
    std::thread                      _worker;
    std::mutex                       _mutex;
    std::condition_variable          _condition;
    bool                             _shouldReturn;
    std::queue<Message>              _backlog;
};

} // namespace scraps::log
