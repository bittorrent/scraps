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

#include <scraps/TaskThread.h>

#include <functional>
#include <future>
#include <list>

namespace scraps {

/**
 * Convenience class to invoke `get()` on futures and passes the result to
 * provided callbacks. Callbacks are always invoked on a different thread.
 */
class FutureSynchronizer {
public:
    template<typename T, typename Callback>
    void push(std::future<T> future, Callback callback);

private:
    TaskThread _taskThread;
};

/**
 * Convenience class to call a callback when a future is ready. Callbacks are
 * only invoked when update() is called. They are invoked on the thread update()
 * was called from. Update should be called frequently.
 */
class PollingFutureSynchronizer {
public:
    /**
     * If the future rethrows an exception, it will rethrow when update() is
     * called. Add any exception handling around calling update.
     */
    template<typename T, typename Callback>
    void push(std::future<T> future, Callback callback);

    void update();

private:
    std::list<std::function<bool()>> _futureChecks;
};

template<typename T, typename Callback>
void FutureSynchronizer::push(std::future<T> future, Callback callback) {
    _taskThread.async([future = std::move(future), callback = std::move(callback)]() mutable {
        callback(future.get());
    });
}

template<typename T, typename Callback>
void PollingFutureSynchronizer::push(std::future<T> future, Callback callback) {
    _futureChecks.emplace_back([future = std::shared_future<T>{std::move(future)}, callback = std::move(callback)]() mutable {
        if (future.wait_for(0s) == std::future_status::ready) {
            callback(future.get());
            return true;
        }
        return false;
    });
}

inline void PollingFutureSynchronizer::update() {
    for (auto i = _futureChecks.begin(); i != _futureChecks.end();) {
        if ((*i)()) {
            i = _futureChecks.erase(i);
        } else {
            ++i;
        }
    }
}

} // namespace scraps
