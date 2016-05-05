#include "scraps/MonotonicScheduler.h"

namespace scraps {

std::chrono::steady_clock::time_point MonotonicScheduler::schedule(std::chrono::steady_clock::time_point remoteTimepoint) noexcept {
    auto localTimepoint = remoteTimepoint + _remoteToLocalOffset;

    _initializeTime();

    if (localTimepoint < *_monotonicTime - _threshold || localTimepoint > *_monotonicTime + _threshold) {
        _resetOffset(*_monotonicTime - localTimepoint + _lastDelta);
        localTimepoint = remoteTimepoint + _remoteToLocalOffset;
    }

    auto prev = *_monotonicTime;
    *_monotonicTime = std::max(*_monotonicTime + 1us, localTimepoint);
    if (*_monotonicTime - prev > 1us) {
        _lastDelta = *_monotonicTime - prev;
    }

    _firstSchedule = false;

    return *_monotonicTime + _localOffsetAdjust;
}

void MonotonicScheduler::reset() noexcept {
    _firstSchedule       = true;
    _monotonicTime       = {};
    _remoteToLocalOffset = {};
}

void MonotonicScheduler::initialize(std::chrono::steady_clock::time_point tp) noexcept {
    reset();
    _monotonicTime = tp;
}

std::chrono::steady_clock::time_point MonotonicScheduler::getTimepoint(std::chrono::steady_clock::duration delta) noexcept {
    _initializeTime();
    *_monotonicTime += delta;
    return *_monotonicTime;
}

void MonotonicScheduler::_resetOffset(std::chrono::steady_clock::duration offset) noexcept {
    _remoteToLocalOffset = offset;
    if (!_firstSchedule) {
        *_monotonicTime += 1us;
        _callback(*_monotonicTime + _localOffsetAdjust);
    }
}

void MonotonicScheduler::_initializeTime() noexcept {
    if (!_monotonicTime) {
        _monotonicTime = std::chrono::steady_clock::now();
    }
}

} // namespace scraps
