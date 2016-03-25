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

    virtual void clear() override;

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
