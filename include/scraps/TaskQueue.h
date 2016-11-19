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

#include <scraps/AbstractTaskScheduler.h>

#include <vector>

namespace scraps {

class TaskQueue : public AbstractTaskScheduler {
public:
    using TaskScope = AbstractTaskScheduler::TaskScope;

    TaskQueue() = default;
    TaskQueue(const TaskQueue&) = delete;
    TaskQueue(TaskQueue&&) = delete;
    TaskQueue& operator=(const TaskQueue&) = delete;
    TaskQueue& operator=(TaskQueue&&) = delete;
    ~TaskQueue() = default;

    void clear();

    /**
    * Call run() often to run all tasks which have been scheduled to be executed before now
    */
    void run();

private:
    virtual void _async(std::unique_ptr<Task> task) override;

    std::mutex                         _mutex;
    std::vector<std::unique_ptr<Task>> _tasks;
};

} // namespace scraps
