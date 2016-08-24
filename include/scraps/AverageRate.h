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

#include "scraps/config.h"
#include "stdts/optional.h"

#include <algorithm>
#include <chrono>
#include <numeric>
#include <map>

namespace scraps {

template <typename Resolution, typename TimePointType, typename ValueType, typename ResultType = decltype(std::declval<ValueType>() / 1.0)>
stdts::optional<ResultType> AverageRate(const std::map<TimePointType, ValueType>& samples, TimePointType begin, TimePointType end);

template <typename Resolution, typename TimePointType, typename ValueType, typename ResultType = decltype(std::declval<ValueType>() / 1.0)>
stdts::optional<ResultType> AverageRate(const std::map<TimePointType, ValueType>& samples, TimePointType begin);

template <typename Resolution, typename TimePointType, typename ValueType, typename ResultType = decltype(std::declval<ValueType>() / 1.0)>
stdts::optional<ResultType> AverageRate(const std::map<TimePointType, ValueType>& samples, stdts::optional<TimePointType> begin = {}, stdts::optional<TimePointType> end = {});

template <typename Resolution, typename IteratorType>
auto AverageRate(IteratorType begin, IteratorType end) -> stdts::optional<decltype(begin->second / 1.0)>;

/**
 * Convenience overloads for template deduction.
 */
template <typename Resolution, typename TimePointType, typename ValueType, typename ResultType>
stdts::optional<ResultType> AverageRate(const std::map<TimePointType, ValueType>& samples, TimePointType begin, TimePointType end) {
    return AverageRate<Resolution, TimePointType, ValueType, ResultType>(samples, stdts::optional<TimePointType>{begin}, stdts::optional<TimePointType>{end});
}

template <typename Resolution, typename TimePointType, typename ValueType, typename ResultType>
stdts::optional<ResultType> AverageRate(const std::map<TimePointType, ValueType>& samples, TimePointType begin) {
    return AverageRate<Resolution, TimePointType, ValueType, ResultType>(samples, stdts::optional<TimePointType>{begin}, stdts::nullopt);
}

namespace detail {

template <typename Resolution, typename IteratorType, typename TimePointType>
auto AverageRateImpl(IteratorType begin, IteratorType end, TimePointType totalDuration)
    -> stdts::optional<decltype(begin->second / 1.0)>
{
    using FloatingPointResolution = std::chrono::duration<double, typename Resolution::period>;
    auto intervalDurationTicks = std::chrono::duration_cast<FloatingPointResolution>(totalDuration).count();

    if (intervalDurationTicks == 0) {
        return {};
    }

    // begin is intentionally left out. It's only used for calculations of the time interval.
    return std::accumulate(std::next(begin), end, decltype(begin->second){}, [](auto carry, auto& kv) { return carry + kv.second; }) / intervalDurationTicks;
}

} // namespace detail

/**
 * Average rate of sampled points covered by the two time points. If the time
 * points exceed the boundaries of the sampled points, 0-value samples are
 * assumed to exist beyond those points and the average rate will be lower as a
 * result.
 */
template <typename Resolution, typename TimePointType, typename ValueType, typename ResultType>
stdts::optional<ResultType> AverageRate(const std::map<TimePointType, ValueType>& samples, stdts::optional<TimePointType> begin, stdts::optional<TimePointType> end) {
    if (!begin && !end) {
        return AverageRate<Resolution>(samples.begin(), samples.end());
    }

    if (samples.size() < 2) {
        return {};
    }

    using SampleIterator = decltype(samples.begin());

    SampleIterator sampleBegin;
    SampleIterator sampleEnd;

    // find iterators of begin and past-the-end
    if (begin) {
        if (*begin >= std::prev(samples.end())->first) {
            return {};
        }
        sampleBegin = samples.lower_bound(*begin);
    } else {
        sampleBegin = samples.begin();
        begin = sampleBegin->first;
    }

    if (end) {
        if (samples.begin()->first >= *end) {
            return {};
        }
        sampleEnd = samples.upper_bound(*end);
    } else {
        sampleEnd = samples.end();
        end = std::prev(samples.end())->first;
    }

    if (*end <= *begin) {
        return {};
    }

    SCRAPS_ASSERT(sampleBegin != samples.end());

    // Two special cases occur with time points that occur inside a single
    // sample or who's end lies at a sample point:
    if (std::prev(sampleEnd)->first == *end && sampleBegin != samples.begin() && sampleBegin == std::prev(sampleEnd)) {
        return AverageRate<Resolution>(std::prev(sampleBegin), sampleEnd);
    }

    if (sampleBegin != samples.begin() && sampleBegin == sampleEnd) {
        return AverageRate<Resolution>(std::prev(sampleBegin), std::next(sampleEnd));
    }

    auto durationEnd = *end > std::prev(samples.end())->first ? *end : std::prev(sampleEnd)->first;
    auto durationBegin = *begin < samples.begin()->first ? *begin : sampleBegin->first;

    return detail::AverageRateImpl<Resolution>(sampleBegin, sampleEnd, durationEnd - durationBegin);
}

/**
 * Average rate of value covered between [begin, end).
 */
template <typename Resolution, typename IteratorType>
auto AverageRate(IteratorType begin, IteratorType end)
    -> stdts::optional<decltype(begin->second / 1.0)>
{
    if (begin == end) {
        return {};
    }

    return detail::AverageRateImpl<Resolution>(begin, end, std::prev(end)->first - begin->first);
}

} // namespace scraps
