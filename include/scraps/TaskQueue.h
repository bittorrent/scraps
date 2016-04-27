#pragma once

#include "scraps/config.h"

#include "scraps/AbstractTaskScheduler.h"

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
