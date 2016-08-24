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

#include "scraps/config.h"

namespace scraps {

template <typename Clock>
class Timer {
public:
    using clock    = Clock;
    using duration = typename clock::duration;

    void start() noexcept {
        if (_stopped) {
            _start   = clock::now();
            _stopped = false;
        }
    }

    void stop() noexcept {
        if (!_stopped) {
            _elapsed += clock::now() - _start;
            _stopped = true;
        }
    }

    void reset() noexcept {
        _elapsed = duration::zero();
        _stopped = true;
    }

    void restart() noexcept {
        _start   = clock::now();
        _elapsed = duration::zero();
        _stopped = false;
    }

    duration elapsed() const noexcept {
        if (_stopped) {
            return _elapsed;
        }
        return _elapsed + clock::now() - _start;
    }

    bool stopped() const noexcept { return _stopped; }

private:
    typename clock::time_point _start;
    duration _elapsed = duration::zero();
    bool _stopped = true;
};

using SystemTimer  = Timer<std::chrono::system_clock>;
using SteadyTimer  = Timer<std::chrono::steady_clock>;
using HighResTimer = Timer<std::chrono::high_resolution_clock>;

} // namespace scraps
