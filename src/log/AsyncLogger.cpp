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
#include <scraps/log/AsyncLogger.h>

namespace scraps::log {

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

void AsyncLogger::log(Message message) {
    {
        std::lock_guard<std::mutex> lock{_mutex};
        _backlog.emplace(message);
    }
    _condition.notify_one();
}

void AsyncLogger::_run() {
    SetThreadName("AsyncLogger");

    std::unique_lock<std::mutex> lock{_mutex};
    while (!_shouldReturn) {
        if (_backlog.empty()) { _condition.wait(lock); }
        while (!_backlog.empty()) {
            auto message = std::move(_backlog.front());
            _backlog.pop();

            lock.unlock(); // unlock to call out
            _logger->log(std::move(message));
            lock.lock();
        }
    }
}

} // namespace scraps::log
