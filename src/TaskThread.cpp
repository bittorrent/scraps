#include "scraps/TaskThread.h"

namespace scraps {

TaskThread::TaskThread(const std::string& threadName)
    : _thread{[=]{ SetThreadName(threadName); _run(); }}
{}

TaskThread::~TaskThread() {
    {
        std::lock_guard<std::mutex> lock{_mutex};
        _exit = true;
    }

    _condVar.notify_one();
    if (_thread.joinable()) {
        _thread.join();
    }
}

void TaskThread::clear() {
    std::lock_guard<std::mutex> lock{_mutex};
    _tasks.clear();
}

void TaskThread::_async(std::unique_ptr<Task> task) {
    auto t = task->time;
    auto firstElement = false;

    {
        std::lock_guard<std::mutex> lock{_mutex};
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
