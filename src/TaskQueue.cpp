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
