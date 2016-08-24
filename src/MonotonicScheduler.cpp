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
#include "scraps/MonotonicScheduler.h"

namespace scraps {

constexpr std::chrono::steady_clock::duration MonotonicScheduler::kDefaultResetOffset;

std::chrono::steady_clock::time_point MonotonicScheduler::schedule(std::chrono::steady_clock::time_point remoteTimePoint) noexcept {
    auto now = _mockSteadyClock ? _mockSteadyClock() : std::chrono::steady_clock::now();
    auto localTimePoint = remoteTimePoint + _remoteToLocalOffset;

    _initializeTime();

    if (_firstSchedule || localTimePoint < now || localTimePoint > *_lastLocalTimePoint + _threshold) {
        SCRAPS_LOG_WARNING("scheduler reset: previous offset {}, estimated local {}, now {}", _remoteToLocalOffset.count(), localTimePoint.time_since_epoch().count(), now.time_since_epoch().count());
        if (!_firstSchedule && (localTimePoint < now - _threshold || localTimePoint > now + _threshold)) {
            *_lastLocalTimePoint += 1us;
            _callback(*_lastLocalTimePoint);
        }
        _remoteToLocalOffset = now - remoteTimePoint;
        localTimePoint = remoteTimePoint + _remoteToLocalOffset;
    }

    _lastLocalTimePoint = std::max(*_lastLocalTimePoint + 1us, localTimePoint);
    _firstSchedule = false;

    return *_lastLocalTimePoint;
}

void MonotonicScheduler::reset() noexcept {
    _firstSchedule       = true;
    _lastLocalTimePoint  = {};
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

void MonotonicScheduler::_initializeTime() noexcept {
    if (!_lastLocalTimePoint) {
        _lastLocalTimePoint = _mockSteadyClock ? _mockSteadyClock() : std::chrono::steady_clock::now();
    }
}

} // namespace scraps
