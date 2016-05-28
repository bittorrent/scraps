#pragma once

#include "config.h"

#include <algorithm>
#include <chrono>

namespace scraps {

/**
 * Cumulative moving average of rate based on sampled time points. Zero duration
 * calculations produce 0, not NaNs.
 */
template <typename ValueType, typename TimePointType = std::chrono::steady_clock::time_point>
class CumulativeRateCalculator {
public:
    void include(TimePointType tp, ValueType val);

    template <typename Resolution>
    ValueType calculate() const;

    void reset();
    uintmax_t samples() const { return _numSamples; }

private:
    TimePointType _begin = {};
    TimePointType _end = {};
    uintmax_t _numSamples = 0;
    ValueType _total = {};
};

template <typename ValueType, typename TimePointType>
void CumulativeRateCalculator<ValueType, TimePointType>::include(TimePointType tp, ValueType val) {
    if (_numSamples == 0) {
        _begin = tp;
        _end = tp;
    } else {
        _begin = std::min(_begin, tp);
        _end = std::max(_end, tp);
    }

    _total = _total + val;
    ++_numSamples;
}

template <typename ValueType, typename TimePointType>
template <typename Resolution>
ValueType CumulativeRateCalculator<ValueType, TimePointType>::calculate() const {
    const auto ticks = std::chrono::duration_cast<Resolution>(_end - _begin).count();
    if (_numSamples > 0 && ticks != 0) {
        return _total / ticks;
    }
    return {};
}

template <typename ValueType, typename TimePointType>
void CumulativeRateCalculator<ValueType, TimePointType>::reset() {
    _begin      = {};
    _end        = {};
    _total      = {};
    _numSamples = 0;
}

} // namespace scraps
