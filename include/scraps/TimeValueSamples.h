#pragma once

#include "scraps/config.h"
#include "scraps/stdts/optional.h"

#include <map>
#include <deque>

namespace scraps {

/**
 * Container for storing timepoints and values for those timepoints. If a maximum size is specified,
 * the container will never exceed that size and the oldest samples (by time point) will be removed.
 */
template <typename ValueType, typename TimePointType = std::chrono::steady_clock::time_point>
class TimeValueSamples {
public:
    TimeValueSamples(stdts::optional<size_t> maxSize = {})
        : _maxSize{maxSize}
    {}

    void insert(TimePointType tp, ValueType val) {
        _samples.emplace(tp, val);
        _ensureSizeInvariant();
    }

    void clear() { _samples.clear(); }
    size_t size() const { return _samples.size(); }
    stdts::optional<size_t> maxSize() const { return _maxSize; }
    const std::map<TimePointType, ValueType>& samples() const { return _samples; }

    std::pair<TimePointType, ValueType> oldest() const { return *_samples.begin(); }
    std::pair<TimePointType, ValueType> newest() const { return *_samples.rbegin(); }

    void setMaxSize(stdts::optional<size_t> maxSize) {
        _maxSize = maxSize;
        _ensureSizeInvariant();
    }

private:
    std::map<TimePointType, ValueType> _samples;
    stdts::optional<size_t> _maxSize;

    void _ensureSizeInvariant() {
        if (!_maxSize) { return; }
        while (*_maxSize < _samples.size()) {
            _samples.erase(_samples.begin());
        }
    }
};

} // namespace scraps
