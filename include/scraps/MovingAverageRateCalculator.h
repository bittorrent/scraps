#pragma once

#include "config.h"
#include "stdts/optional.h"

#include <algorithm>
#include <chrono>
#include <numeric>
#include <map>

namespace scraps {

/**
 * Moving average of rate based on sampled time points. If the target number of
 * samples isn't met or the samples included would cover a zero duration
 * interval nullopt is returned instead. Sample size must be greater than 1.
 */
template <typename ValueType, typename TimePointType = std::chrono::steady_clock::time_point>
class MovingAverageRateCalculator {
public:
    MovingAverageRateCalculator(size_t sampleSize);

    void include(TimePointType tp, ValueType val);

    template <typename Resolution>
    stdts::optional<ValueType> calculate() const;

    void reset();
    uintmax_t currentSize() const { return _samples.size(); }
    void resize(size_t sampleSize);

private:
    std::map<TimePointType, ValueType> _samples;
    size_t _sampleSize = 0;

    void _ensureSizeInvariant();
};


template <typename ValueType, typename TimePointType>
MovingAverageRateCalculator<ValueType, TimePointType>::MovingAverageRateCalculator(size_t sampleSize)
    : _sampleSize{sampleSize}
{}

template <typename ValueType, typename TimePointType>
void MovingAverageRateCalculator<ValueType, TimePointType>::include(TimePointType tp, ValueType val) {
    _samples.emplace(tp, val);
    _ensureSizeInvariant();
}

template <typename ValueType, typename TimePointType>
template <typename Resolution>
stdts::optional<ValueType> MovingAverageRateCalculator<ValueType, TimePointType>::calculate() const {
    SCRAPS_ASSERT(_sampleSize >= 2);
    if (_samples.size() != _sampleSize) {
        return {};
    }

    using FloatingPointResolution = std::chrono::duration<double, typename Resolution::period>;

    auto intervalStart = _samples.begin()->first;
    auto intervalEnd   = (_samples.rbegin())->first;

    auto intervalDurationTicks = std::chrono::duration_cast<FloatingPointResolution>(intervalEnd - intervalStart).count();

    if (intervalDurationTicks == 0) {
        return {};
    }

    return std::accumulate(_samples.begin(), --_samples.end(), ValueType{}, [](auto carry, auto& kv) { return carry + kv.second; }) / intervalDurationTicks;
}

template <typename ValueType, typename TimePointType>
void MovingAverageRateCalculator<ValueType, TimePointType>::resize(size_t sampleSize) {
    _sampleSize = sampleSize;
    SCRAPS_ASSERT(_sampleSize >= 2);
    _ensureSizeInvariant();
}

template <typename ValueType, typename TimePointType>
void MovingAverageRateCalculator<ValueType, TimePointType>::reset() {
    _samples.clear();
}

template <typename ValueType, typename TimePointType>
void MovingAverageRateCalculator<ValueType, TimePointType>::_ensureSizeInvariant() {
    while (_sampleSize < _samples.size()) {
        _samples.erase(_samples.begin());
    }
}

} // namespace scraps
