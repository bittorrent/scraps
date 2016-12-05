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

#include "scraps/config.h"

#include "scraps/AbstractTaskScheduler.h"

#include <condition_variable>
#include <vector>
#include <algorithm>

namespace scraps {

class TaskThread : public AbstractTaskScheduler {
public:
    using TaskScope = AbstractTaskScheduler::TaskScope;

    explicit TaskThread(const std::string& threadName = "TaskThread");
    TaskThread(const TaskThread&) = delete;
    TaskThread(TaskThread&&) = delete;
    TaskThread& operator=(const TaskThread&) = delete;
    TaskThread& operator=(TaskThread&&) = delete;
    ~TaskThread();

    void cancelAndJoin();

private:
    virtual void _async(std::unique_ptr<Task> task) override;
    void _run();

    std::mutex                         _mutex;
    std::vector<std::unique_ptr<Task>> _tasks;
    std::condition_variable            _condVar;
    bool                               _exit = false;
    std::thread                        _thread;
};

} // namespace scraps
