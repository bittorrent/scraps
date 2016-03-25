#pragma once

#include "scraps/config.h"

#include <atomic>
#include <future>
#include <memory>
#include <mutex>
#include <type_traits>
#include <unordered_set>

namespace scraps {

class AbstractTaskScheduler {
public:
    class TaskScope;

    AbstractTaskScheduler() = default;
    AbstractTaskScheduler(const AbstractTaskScheduler&) = delete;
    AbstractTaskScheduler(AbstractTaskScheduler&&) = delete;
    AbstractTaskScheduler& operator=(const AbstractTaskScheduler&) = delete;
    AbstractTaskScheduler& operator=(AbstractTaskScheduler&&) = delete;
    virtual ~AbstractTaskScheduler() = default;

    template<class F, class... Args>
    std::future<typename std::result_of<typename std::decay<F>::type(typename std::decay<Args>::type...)>::type>
    async(F&& f, Args&&... args);

    template<class F, class... Args>
    std::future<typename std::result_of<typename std::decay<F>::type(typename std::decay<Args>::type...)>::type>
    async(const TaskScope& scope, F&& f, Args&&... args);

    template<class F, class... Args>
    std::future<typename std::result_of<typename std::decay<F>::type(typename std::decay<Args>::type...)>::type>
    asyncAfter(const std::chrono::steady_clock::duration& d, F&& f, Args&&... args);

    template<class F, class... Args>
    std::future<typename std::result_of<typename std::decay<F>::type(typename std::decay<Args>::type...)>::type>
    asyncAfter(const TaskScope& scope, const std::chrono::steady_clock::duration& d, F&& f, Args&&... args);

    template<class F, class... Args>
    std::future<typename std::result_of<typename std::decay<F>::type(typename std::decay<Args>::type...)>::type>
    asyncAt(const std::chrono::steady_clock::time_point& t, F&& f, Args&&... args);

    template<class F, class... Args>
    std::future<typename std::result_of<typename std::decay<F>::type(typename std::decay<Args>::type...)>::type>
    asyncAt(const TaskScope& scope, const std::chrono::steady_clock::time_point& t, F&& f, Args&&... args);

    virtual void clear() = 0;

protected:
    struct Task;
    template<class> struct TaskImpl;
    struct TaskComparer;
    struct TaskScopeImpl;

private:
    template<class F, class... Args>
    std::packaged_task<typename std::result_of<typename std::decay<F>::type(typename std::decay<Args>::type...)>::type()>
    _makePackagedTask(F&& f, Args&&... args);

    template<class R, class... Args>
    std::unique_ptr<Task>
    _makeTask(std::chrono::steady_clock::time_point t, std::packaged_task<R(Args...)>&& task, std::shared_ptr<TaskScopeImpl> scope = {});

    virtual void _async(std::unique_ptr<Task> task) = 0;
};

struct AbstractTaskScheduler::TaskScopeImpl {
    ~TaskScopeImpl() { reset(); }
    void reset();

    std::mutex                mutex;
    std::unordered_set<Task*> tasks;
};

class AbstractTaskScheduler::TaskScope {
public:
    TaskScope() : _scope{std::make_shared<TaskScopeImpl>()} {}
    TaskScope(const TaskScope&) = delete;
    TaskScope& operator=(const TaskScope&) = delete;
    ~TaskScope() { if (_scope) _scope->reset(); }

    void reset() { _scope->reset(); _scope = std::make_shared<TaskScopeImpl>(); }
    void endScope() { _scope->reset(); _scope.reset(); }

private:
    friend class AbstractTaskScheduler;
    std::shared_ptr<TaskScopeImpl> _scope;
};

struct AbstractTaskScheduler::Task {
    Task(std::chrono::steady_clock::time_point time) : time{time} {}
    virtual ~Task() {}

    virtual void operator()() = 0;
    virtual void reset() = 0;

    std::chrono::steady_clock::time_point time;
};

template<class R, class... Args>
struct AbstractTaskScheduler::TaskImpl<R(Args...)> : public AbstractTaskScheduler::Task {
    TaskImpl(std::chrono::steady_clock::time_point time, std::packaged_task<R(Args...)>&& task, std::shared_ptr<TaskScopeImpl> scope = {})
        : Task(time)
        , task{std::move(task)}
        , weakScope{scope}
    {
        if (scope) {
            std::lock_guard<std::mutex> lock{scope->mutex};
            scope->tasks.insert(this);
        }
    }

    virtual ~TaskImpl() { _removeFromTaskScope(); }

    virtual void operator()() override {
        {
            std::lock_guard<std::mutex> lock{mutex};
            if (task.valid()) {
                task();
                task = {};
            }
        }

        _removeFromTaskScope();
    }

    virtual void reset() override {
        std::lock_guard<std::mutex> lock(mutex);
        task = {};
    }

    void _removeFromTaskScope() {
        auto scope = weakScope.lock();
        if (scope) {
            std::lock_guard<std::mutex> lock{scope->mutex};
            scope->tasks.erase(this);
        }
    }

    std::mutex                     mutex;
    std::packaged_task<R(Args...)> task;
    std::weak_ptr<TaskScopeImpl>   weakScope;
};

struct AbstractTaskScheduler::TaskComparer {
    bool operator()(const std::unique_ptr<Task>& left, const std::unique_ptr<Task>& right) const {
        return right->time < left->time;
    }

    bool operator()(const Task& left, const Task& right) const {
        return right.time < left.time;
    }
};

inline void AbstractTaskScheduler::TaskScopeImpl::reset() {
    std::lock_guard<std::mutex> lock{mutex};

    for (auto task : tasks) {
        task->reset();
    }

    tasks.clear();
}

template<class F, class... Args>
std::future<typename std::result_of<typename std::decay<F>::type(typename std::decay<Args>::type...)>::type>
AbstractTaskScheduler::async(F&& f, Args&&... args) {
    return asyncAt(std::chrono::steady_clock::now(), std::forward<F>(f), std::forward<Args>(args)...);
}

template<class F, class... Args>
std::future<typename std::result_of<typename std::decay<F>::type(typename std::decay<Args>::type...)>::type>
AbstractTaskScheduler::async(const TaskScope& scope, F&& f, Args&&... args) {
    return asyncAt(scope, std::chrono::steady_clock::now(), std::forward<F>(f), std::forward<Args>(args)...);
}

template<class F, class... Args>
std::future<typename std::result_of<typename std::decay<F>::type(typename std::decay<Args>::type...)>::type>
AbstractTaskScheduler::asyncAfter(const std::chrono::steady_clock::duration& d, F&& f, Args&&... args) {
    return asyncAt(std::chrono::steady_clock::now() + d, std::forward<F>(f), std::forward<Args>(args)...);
}

template<class F, class... Args>
std::future<typename std::result_of<typename std::decay<F>::type(typename std::decay<Args>::type...)>::type>
AbstractTaskScheduler::asyncAfter(const TaskScope& scope, const std::chrono::steady_clock::duration& d, F&& f, Args&&... args) {
    return asyncAt(scope, std::chrono::steady_clock::now() + d, std::forward<F>(f), std::forward<Args>(args)...);
}

template<class F, class... Args>
std::future<typename std::result_of<typename std::decay<F>::type(typename std::decay<Args>::type...)>::type>
AbstractTaskScheduler::asyncAt(const std::chrono::steady_clock::time_point& t, F&& f, Args&&... args) {
    auto task = _makePackagedTask(std::forward<F>(f), std::forward<Args>(args)...);
    auto future = task.get_future();
    _async(_makeTask(t, std::move(task)));
    return future;
}

template<class F, class... Args>
std::future<typename std::result_of<typename std::decay<F>::type(typename std::decay<Args>::type...)>::type>
AbstractTaskScheduler::asyncAt(const TaskScope& scope, const std::chrono::steady_clock::time_point& t, F&& f, Args&&... args) {
    auto task = _makePackagedTask(std::forward<F>(f), std::forward<Args>(args)...);
    auto future = task.get_future();
    auto s = scope._scope;
    if (s) {
        _async(_makeTask(t, std::move(task), std::move(s)));
    }
    return future;
}

template<class F, class... Args>
std::packaged_task<typename std::result_of<typename std::decay<F>::type(typename std::decay<Args>::type...)>::type()>
AbstractTaskScheduler::_makePackagedTask(F&& f, Args&&... args) {
    using TaskType = std::packaged_task<typename std::result_of<typename std::decay<F>::type(typename std::decay<Args>::type...)>::type()>;
    return TaskType(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
}

template<class R, class... Args>
std::unique_ptr<AbstractTaskScheduler::Task>
AbstractTaskScheduler::_makeTask(std::chrono::steady_clock::time_point t, std::packaged_task<R(Args...)>&& task, std::shared_ptr<TaskScopeImpl> scope) {
    return std::make_unique<TaskImpl<R(Args...)>>(t, std::move(task), scope);
}

} // namespace scraps
