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
