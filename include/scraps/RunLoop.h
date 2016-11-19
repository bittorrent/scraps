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

#include <functional>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include <poll.h>

#include <atomic>
#include <queue>

namespace scraps {

/**
* Thread-safe.
*/
class RunLoop {
public:
    /**
    * Runs the poll loop. Blocks until an error occurs or cancel is invoked.
    */
    void run();

    /**
    * Sets the event handler.
    */
    void setEventHandler(const std::function<void(int fd, short events)>& eventHandler);

    /**
    * Sets a function to be invoked by the poll loop after a specified delay.
    */
    void async(const std::function<void()>& func, std::chrono::milliseconds delay = std::chrono::milliseconds(0));

    /**
    * Finish running any outstanding functions left in the queue. This should only be needed
    * after cancel() has been called and there are still tasks that need to be executed.
    */
    void flush();

    /**
    * Causes run() to return.
    */
    void cancel();

    /**
    * Adds the file descriptor to the loop.
    * @param fd the file descriptor
    * @param events the poll events to watch for (POLLIN, POLLHUP, etc.)
    */
    void add(int fd, short events);

    /**
    * Removes the file descriptor from the loop.
    */
    void remove(int fd);

    /**
    * Resets the loop to its post-construction state. This function must not be
    * called prior to a call to cancel.
    */
    void reset();

private:
    std::mutex _mutex;

    std::vector<struct pollfd> _pollDescriptors;

    std::function<void(int fd, short events)> _eventHandler;

    std::unordered_map<int, short> _pendingAdditions;
    std::unordered_set<int> _pendingRemovals;

    struct AsyncFunction {
        AsyncFunction(const std::function<void()>& func, std::chrono::steady_clock::time_point time, uint64_t order) : func(func), time(time), order(order) {}
        std::function<void()> func;
        std::chrono::steady_clock::time_point time;
        uint64_t order = 0;
        bool operator<(const AsyncFunction& other) const { return time == other.time ? (order > other.order) : (time > other.time); }
    };

    std::priority_queue<AsyncFunction> _asyncFunctions;
    uint64_t _asyncFunctionCounter = 0;

    std::atomic<bool> _isCancelled{false};

    std::mutex _pipeMutex;
    enum { kWakeUpPipeOutput, kWakeUpPipeInput };
    int _wakeUpPipe[2]{-1, -1};
    std::atomic<bool> _shouldWake{false};

    void _wakeUp();
    void _processPendingAdditionsAndRemovals();
};

} // namespace scraps
