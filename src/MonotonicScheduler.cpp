#include "scraps/MonotonicScheduler.h"

namespace scraps {

std::chrono::steady_clock::time_point MonotonicScheduler::schedule(std::chrono::steady_clock::time_point remoteTimePoint) noexcept {
    auto localTimePoint = remoteTimePoint + _remoteToLocalOffset;

    _initializeTime();

    if (_firstSchedule ||
        (_lastRemoteTimePoint && (remoteTimePoint - *_lastRemoteTimePoint > _threshold || *_lastRemoteTimePoint - remoteTimePoint > _threshold))
    ) {
        _resetOffset(*_lastLocalTimePoint - localTimePoint + _lastDelta);
        localTimePoint = remoteTimePoint + _remoteToLocalOffset;
    }

    _lastDelta = remoteTimePoint - _lastRemoteTimePoint.value_or(remoteTimePoint);
    _lastRemoteTimePoint = remoteTimePoint;
    _lastLocalTimePoint = std::max(*_lastLocalTimePoint + 1us, localTimePoint);
    _firstSchedule = false;

    return *_lastLocalTimePoint;
}

void MonotonicScheduler::reset() noexcept {
    _firstSchedule       = true;
    _lastLocalTimePoint       = {};
    _remoteToLocalOffset = {};
}

void MonotonicScheduler::initialize(std::chrono::steady_clock::time_point tp) noexcept {
    reset();
    _lastLocalTimePoint = tp;
}

std::chrono::steady_clock::time_point MonotonicScheduler::getTimePoint(std::chrono::steady_clock::duration delta) noexcept {
    _initializeTime();
    *_lastLocalTimePoint += delta;
    return *_lastLocalTimePoint;
}

void MonotonicScheduler::synchronizeWith(const MonotonicScheduler& other) noexcept {
    _firstSchedule = other._firstSchedule;
    _remoteToLocalOffset = other._remoteToLocalOffset;
}

void MonotonicScheduler::_resetOffset(std::chrono::steady_clock::duration offset) noexcept {
    _remoteToLocalOffset = offset;
    if (!_firstSchedule) {
        *_lastLocalTimePoint += 1us;
        _callback(*_lastLocalTimePoint);
    }
}

void MonotonicScheduler::_initializeTime() noexcept {
    if (!_lastLocalTimePoint) {
        _lastLocalTimePoint = std::chrono::steady_clock::now();
    }
}

} // namespace scraps
