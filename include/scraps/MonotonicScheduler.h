#pragma once

#include "scraps/config.h"
#include "scraps/stdts/optional.h"

#include <chrono>
#include <functional>
#include <algorithm>

namespace scraps {

/**
 * Converts timepoints in a streaming input set to an offset-corrected monotonic
 * local timepoint. Can be used to normalize remote time points that may change
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
 * the constructor or is out of order with respect to previous timepoints, then
 * the internal offset is reset.
 *
 * Example (1 input per second and a forward-in-time reset):
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
 * provided with a unique monotonic timepoint to come before the timepoint that
 * caused the reset.
 */
class MonotonicScheduler {
public:
    using CallbackType = std::function<void(std::chrono::steady_clock::time_point)>;

    MonotonicScheduler(std::chrono::steady_clock::duration threshold, CallbackType callback = [](auto){})
        : _callback{std::move(callback)}
        , _threshold{threshold}
    {}

    std::chrono::steady_clock::time_point schedule(std::chrono::steady_clock::time_point remoteTimepoint) noexcept;

    /**
     * Reset internal state and keey settings.
     */
    void reset() noexcept;

    /**
     * Reset and add an initial time point.
     */
    void initialize(std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now()) noexcept;

    /**
     * Return the a monotonic timepoint.
     */
    std::chrono::steady_clock::time_point getTimepoint(std::chrono::steady_clock::duration delta = 1us) noexcept;

    /**
     * Return the current time offset.
     */
    std::chrono::steady_clock::duration offset() const noexcept { return _remoteToLocalOffset; }

    /**
     * True if monotonic time has a value
     */
    bool initialized() const noexcept { return static_cast<bool>(_monotonicTime); }

    /**
     * When set, adjusts the schedule outputs by `offset` amount.
     */
    void adjustLocalTime(std::chrono::steady_clock::duration offset) { _localOffsetAdjust = offset; }

private:
    bool _firstSchedule = true;
    CallbackType _callback;
    std::chrono::steady_clock::duration _threshold           = {};
    std::chrono::steady_clock::duration _lastDelta           = {};
    std::chrono::steady_clock::duration _remoteToLocalOffset = {};
    std::chrono::steady_clock::duration _localOffsetAdjust   = {};
    stdts::optional<std::chrono::steady_clock::time_point> _monotonicTime;

    void _resetOffset(std::chrono::steady_clock::duration offset) noexcept;
    void _initializeTime() noexcept;
};

} // namespace scraps
