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

#include <scraps/config.h>
#include <stdts/optional.h>

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
