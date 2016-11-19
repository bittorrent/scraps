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
#include <scraps/RunLoop.h>

#include <scraps/logging.h>
#include <scraps/utility.h>

#include <gsl.h>

#include <unistd.h>

#include <cassert>

namespace scraps {

void RunLoop::run() {
    {
        std::lock_guard<std::mutex> lk{_pipeMutex};

        _wakeUpPipe[kWakeUpPipeInput] = -1;
        _wakeUpPipe[kWakeUpPipeOutput] = -1;

        if (pipe(_wakeUpPipe)) {
            SCRAPS_LOGF_ERROR("error creating wake up pipe (errno = %d)", static_cast<int>(errno));
            return;
        }
    }

    auto _ = gsl::finally([&] {
        std::lock_guard<std::mutex> lk{_pipeMutex};
        for (auto& fd : _wakeUpPipe) {
            close(fd);
            fd = -1;
        }
    });

    if (!SetBlocking(_wakeUpPipe[kWakeUpPipeInput], false) || !SetBlocking(_wakeUpPipe[kWakeUpPipeOutput], false)) {
        SCRAPS_LOGF_ERROR("error making pipe non-blocking (errno = %d)", static_cast<int>(errno));
        return;
    }

    {
        _pollDescriptors.emplace_back();
        auto& pfd = _pollDescriptors.back();
        pfd.fd = _wakeUpPipe[kWakeUpPipeOutput];
        pfd.events = POLLIN;
    }

    _processPendingAdditionsAndRemovals();

    while (!_isCancelled) {
        int timeout = -1;
        {
            std::lock_guard<std::mutex> lock{_mutex};
            if (!_asyncFunctions.empty()) {
                const auto now = std::chrono::steady_clock::now();
                auto& next = _asyncFunctions.top();
                std::chrono::milliseconds delay{0};
                if (next.time > now) {
                    delay = std::chrono::duration_cast<std::chrono::milliseconds>(next.time - now);
                }
                timeout = delay.count() < 0 ? 0 : delay.count();
            }
        }

        auto result = poll(_pollDescriptors.data(), _pollDescriptors.size(), timeout);

        if (result < 0) {
            SCRAPS_LOGF_ERROR("error polling sockets (fds = %zu, errno = %d)", _pollDescriptors.size(), static_cast<int>(errno));
            break;
        }

        _processPendingAdditionsAndRemovals();

        auto remaining = result;
        for (auto it = _pollDescriptors.begin(); remaining > 0 && it != _pollDescriptors.end(); ++it) {
            if (!it->revents) { continue; }
            --remaining;

            if (it->fd == _wakeUpPipe[kWakeUpPipeOutput]) {
                char buf[10];
                while (read(it->fd, buf, sizeof(buf)) > 0);
                _shouldWake = false;
                continue;
            }

            if (_eventHandler) {
                _eventHandler(it->fd, it->revents);
            }
        }

        std::vector<std::function<void()>> asyncFunctionsToCall;

        {
            std::lock_guard<std::mutex> lock{_mutex};
            auto now = std::chrono::steady_clock::now();
            while (!_asyncFunctions.empty()) {
                auto& next = _asyncFunctions.top();
                if (next.time > now) { break; }
                asyncFunctionsToCall.emplace_back(std::move(next.func));
                _asyncFunctions.pop();
            }
        }

        for (auto& func : asyncFunctionsToCall) {
            func();
        }

        _processPendingAdditionsAndRemovals();
    }
}

void RunLoop::setEventHandler(const std::function<void(int fd, short events)>& eventHandler) {
    _eventHandler = eventHandler;
}

void RunLoop::async(const std::function<void()>& func, std::chrono::milliseconds delay) {
    {
        std::lock_guard<std::mutex> lock{_mutex};
        _asyncFunctions.emplace(func, delay.count() ? std::chrono::steady_clock::now() + delay : std::chrono::steady_clock::time_point::min(), ++_asyncFunctionCounter);
    }
    _wakeUp();
}

void RunLoop::flush() {

    // Save now for the first loop, it shouldn't be changed after that.
    // Any subsequent calls to async will be processed after the second
    // loop only if they have no delay.
    //
    // next.time == std::chrono::steady_clock::time_point::min(), which is < now.
    //
    // Otherwise any async func added with a delay will have a time > now and will
    // not be processed.
    std::chrono::steady_clock::time_point now;
    for (bool first = true;;) {
        std::vector<std::function<void()>> asyncFunctionsToCall;

        {
            std::lock_guard<std::mutex> lock{_mutex};
            if (first) {
                now = std::chrono::steady_clock::now();
            }

            for (; !_asyncFunctions.empty(); _asyncFunctions.pop()) {
                auto& next = _asyncFunctions.top();
                if (next.time > now) { break; }

                asyncFunctionsToCall.emplace_back(std::move(next.func));
            }
        }

        if (asyncFunctionsToCall.empty()) { return; }

        for (auto& func : asyncFunctionsToCall) {
            func();
        }

        if (first) {
            first = false;
        }
    }
}

void RunLoop::cancel() {
    _isCancelled = true;
    _wakeUp();
}

void RunLoop::add(int fd, short events) {
    {
        std::lock_guard<std::mutex> lock{_mutex};
        _pendingAdditions[fd] = events;
        _pendingRemovals.erase(fd);
    }
    _wakeUp();
}

void RunLoop::remove(int fd) {
    {
        std::lock_guard<std::mutex> lock{_mutex};
        _pendingAdditions.erase(fd);
        _pendingRemovals.insert(fd);
    }
    _wakeUp();
}

void RunLoop::reset() {
    std::lock(_mutex, _pipeMutex);
    std::lock_guard<std::mutex> lk1(_mutex, std::adopt_lock);
    std::lock_guard<std::mutex> lk2(_pipeMutex, std::adopt_lock);

    assert(_isCancelled);
    _isCancelled = false;

    _wakeUpPipe[0] = -1;
    _wakeUpPipe[1] = -1;

    _pendingAdditions.clear();
    _pendingRemovals.clear();
    while (!_asyncFunctions.empty()) {
        _asyncFunctions.pop();
    }
}

void RunLoop::_wakeUp() {
    std::lock_guard<std::mutex> lk{_pipeMutex};
    if (_wakeUpPipe[kWakeUpPipeInput] < 0) { return; }
    if (_shouldWake.exchange(true)) {
        // we set _shouldWake to true when we want to wake
        // we set _shouldWake to false after waking, but before doing anything
        // so if _shouldWake was already true, this method has already been called since the last wake-up, and
        // writing to the pipe would be excessive
        return;
    }
    if (write(_wakeUpPipe[kWakeUpPipeInput], "!", 1) != 1) {
        SCRAPS_LOGF_ERROR("error writing to wake up pipe (errno = %d)", static_cast<int>(errno));
    }
}

void RunLoop::_processPendingAdditionsAndRemovals() {
    std::lock_guard<std::mutex> lock{_mutex};

    for (auto it = _pollDescriptors.begin(); !_pendingRemovals.empty() && it != _pollDescriptors.end(); ++it) {
        if (_pendingRemovals.erase(it->fd)) {
            *it = std::move(_pollDescriptors.back());
            _pollDescriptors.pop_back();
            continue;
        }
        auto pait = _pendingAdditions.find(it->fd);
        if (pait != _pendingAdditions.end()) {
            it->events = pait->second;
            _pendingAdditions.erase(pait);
        }
    }

    for (auto& kv : _pendingAdditions) {
        bool existed = false;
        for (auto& pfd : _pollDescriptors) {
            if (pfd.fd == kv.first) {
                existed = true;
                pfd.events = kv.second;
                break;
            }
        }
        if (!existed) {
            _pollDescriptors.emplace_back();
            auto& pfd = _pollDescriptors.back();
            pfd.fd = kv.first;
            pfd.events = kv.second;
        }
    }

    _pendingAdditions.clear();
    _pendingRemovals.clear();
}

} // namespace scraps
