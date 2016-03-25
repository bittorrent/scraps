#pragma once

#include "scraps/config.h"

#include "scraps/TaskThread.h"

#include <functional>
#include <future>

namespace scraps {

class FutureSynchronizer {
public:
    template<typename T, typename Callback>
    void push(std::future<T> future, Callback callback) {
        _taskThread.async([future = std::move(future), callback = std::move(callback)]() mutable {
            callback(future.get());
        });
    }

private:
    TaskThread _taskThread;
};

} // namespace scraps
