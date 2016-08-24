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
#include "scraps/TaskThread.h"

namespace scraps {

TaskThread::TaskThread(const std::string& threadName)
    : _thread{[=]{ SetThreadName(threadName); _run(); }}
{}

TaskThread::~TaskThread() {
    cancelAndJoin();
}

void TaskThread::cancelAndJoin() {
    {
        std::lock_guard<std::mutex> lock{_mutex};
        _exit = true;
    }

    _condVar.notify_one();
    if (_thread.joinable()) {
        _thread.join();
    }
    _tasks.clear();
}

void TaskThread::_async(std::unique_ptr<Task> task) {
    auto t = task->time;
    auto firstElement = false;

    {
        std::lock_guard<std::mutex> lock{_mutex};
        if (_exit) { return; }
        _tasks.push_back(std::move(task));
        std::push_heap(_tasks.begin(), _tasks.end(), TaskComparer{});
        firstElement = _tasks.front()->time == t;
    }

    if (firstElement) {
        _condVar.notify_one();
    }
}

void TaskThread::_run() {
    std::unique_lock<std::mutex> lock{_mutex};

    while (!_exit) {
        if (_tasks.empty()) {
            _condVar.wait(lock, [&]{ return _exit || !_tasks.empty(); });
        } else {
            auto next = _tasks.front()->time;
            _condVar.wait_until(lock, next, [&]{ return _exit || _tasks.empty() || _tasks.front()->time != next; });
        }

        while (!_exit && !_tasks.empty() && _tasks.front()->time <= std::chrono::steady_clock::now()) {
            std::pop_heap(_tasks.begin(), _tasks.end(), TaskComparer{});
            auto task = std::move(_tasks.back());
            _tasks.pop_back();

            lock.unlock();
            (*task)();
            lock.lock();
        }
    }
}

} // namespace scraps
