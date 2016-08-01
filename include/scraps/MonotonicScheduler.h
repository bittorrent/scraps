#pragma once

#include "scraps/config.h"
#include "stdts/optional.h"

#include <chrono>
#include <functional>
#include <algorithm>

namespace scraps {

/**
 * Converts time points in a streaming input set to an offset-corrected monotonic
 * local time point. Can be used to normalize remote time points that may change
 * monotonicity occasionally.
 *
 * Example (1 input per second):
 *   input:  3, 4, 5, 6, 7
 *   output: 1, 2, 3, 4, 5
 *          ^
 *          offset = -2
 *
 * As the output offset is calculated on first run, inputs should be scheduled
 * in realtime. If an input would ever exceed the threshold provided to
 * the constructor or is out of order with respect to previous time points, then
 * the internal offset is reset and the timepoint incremented by the steady_clock
 * epsilon + an optional delta.
 *
 * Example (1 input per second with a forward-in-time reset):
 *   input:  1, 2, 10, 11, 12, 13
 *   output: 1, 2,  3,  4,  5,  6,
 *                ^
 *                reset point
 *
 * Example (1 input per second and a backward-in-time reset):
 *   input:  5, 6, 10, 1, 2, 3
 *   output: 1, 2,  6, 7, 8, 9
 *                    ^
 *                    reset point
 *
 * If the offset is ever reset, the provided callback will be triggered and
 * provided with a unique monotonic time point to come before the time point that
 * caused the reset.
 */
class MonotonicScheduler {
public:
    static constexpr std::chrono::steady_clock::duration kDefaultResetOffset = 100ms;

    using CallbackType = std::function<void(std::chrono::steady_clock::time_point)>;

    /**
     * @param threshold The scheduler is reset if input is outside of the bounds now -> now + threshold.
     * @param callback If the scheduler is reset more than once (1 freebie), callback is invoked.
     * @param postResetOffset If the output is reset, the first new output will occur at now + postResetOffset.
     */
    MonotonicScheduler(std::chrono::steady_clock::duration threshold, CallbackType callback = [](auto){})
        : _callback{std::move(callback)}
        , _threshold{threshold}
    {}

    /**
    * For testing, you can provide a mock steady clock here.
    */
    void mockSteadyClock(std::function<std::chrono::steady_clock::time_point()> mockSteadyClock) { _mockSteadyClock = std::move(mockSteadyClock); }

    std::chrono::steady_clock::time_point schedule(std::chrono::steady_clock::time_point remoteTimePoint) noexcept;

    /**
     * Reset internal state and keep settings.
     */
    void reset() noexcept;

    /**
     * Reset to an initial time point.
     */
    void initialize(std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now()) noexcept;

    /**
     * Return the a monotonic time point and increments the internal counter.
     */
    std::chrono::steady_clock::time_point getTimePoint(std::chrono::steady_clock::duration delta = 1us) noexcept;

    /**
     * Return current remote to local offset.
     */
    std::chrono::steady_clock::duration offset() const noexcept { return _remoteToLocalOffset; }

    /**
     * True if monotonic time has a value.
     */
    bool initialized() const noexcept { return static_cast<bool>(_lastLocalTimePoint); }

    /**
    * Forces the scheduler to use the same remote epoch as other.
    */
    void synchronizeWith(const MonotonicScheduler& other) noexcept;

private:
    bool _firstSchedule = true;
    CallbackType _callback;
    std::chrono::steady_clock::duration                    _threshold           = {};
    std::chrono::steady_clock::duration                    _remoteToLocalOffset = {};
    stdts::optional<std::chrono::steady_clock::time_point> _lastLocalTimePoint;
    std::function<std::chrono::steady_clock::time_point()> _mockSteadyClock;

    void _initializeTime() noexcept;
};

} // namespace scraps
