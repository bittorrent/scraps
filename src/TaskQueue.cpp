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
#include "scraps/TaskQueue.h"

namespace scraps {

void TaskQueue::clear() {
    std::lock_guard<std::mutex> lock{_mutex};
    _tasks.clear();
}

void TaskQueue::_async(std::unique_ptr<Task> task) {
    std::lock_guard<std::mutex> lock{_mutex};
    _tasks.push_back(std::move(task));
    std::push_heap(_tasks.begin(), _tasks.end(), TaskComparer{});
}

void TaskQueue::run() {
    std::unique_lock<std::mutex> lock{_mutex};

    while (!_tasks.empty() && _tasks.front()->time <= std::chrono::steady_clock::now()) {
        std::pop_heap(_tasks.begin(), _tasks.end(), TaskComparer{});
        auto task = std::move(_tasks.back());
        _tasks.pop_back();

        lock.unlock();
        (*task)();
        lock.lock();
    }
}

} // namespace scraps
