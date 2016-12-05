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

#include <vector>
#include <random>
#include <chrono>
#include <cstdint>

namespace scraps {

/**
* Produces integer values evenly distributed across a range
*/
template <typename T, typename U = T, typename Generator>
typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, typename std::common_type<T, U>::type>::type
UniformDistribution(Generator& g, T min = 0, U max = std::numeric_limits<U>::max()) {
    return std::uniform_int_distribution<typename std::common_type<T, U>::type>(min, max)(g);
}

/**
* Produces real values evenly distributed across a range
*/
template <typename T, typename U = T, typename Generator>
typename std::enable_if<std::is_floating_point<T>::value && std::is_floating_point<U>::value, typename std::common_type<T, U>::type>::type
UniformDistribution(Generator& g, T min = 0.0, U max = 1.0) {
    return std::uniform_real_distribution<typename std::common_type<T, U>::type>(min, max)(g);
}

/**
* Produces chrono::durartion values evenly distributed across a range
*/
template <typename Rep1, typename Period1, typename Rep2, typename Period2, typename Generator>
typename std::common_type<std::chrono::duration<Rep1, Period1>, std::chrono::duration<Rep2, Period2>>::type
UniformDistribution(Generator& g, const std::chrono::duration<Rep1, Period1>& min, const std::chrono::duration<Rep2, Period2>& max) {
    using CommonType = typename std::common_type<std::chrono::duration<Rep1, Period1>, std::chrono::duration<Rep2, Period2>>::type;
    return CommonType{UniformDistribution(g, CommonType{min}.count(), CommonType{max}.count())};
}

/**
* Produces chromo::time_point values evenly distributed across a range
*/
template <typename Clock, typename Duration1, typename Duration2, typename Generator>
typename std::common_type<std::chrono::time_point<Clock, Duration1>, std::chrono::time_point<Clock, Duration2>>::type
UniformDistribution(Generator& g, const std::chrono::time_point<Clock, Duration1>& min, const std::chrono::time_point<Clock, Duration2>& max) {
    using CommonType = typename std::common_type<std::chrono::time_point<Clock, Duration1>, std::chrono::time_point<Clock, Duration2>>::type;
    return CommonType{UniformDistribution(g, min.time_since_epoch(), max.time_since_epoch())};
}

/**
* Returns a vector with n random non-duplicate iterators to elements in the input range, with
* uniform probability, in O(n) time. n must be smaller than the input range length.
*
* Only the chosen elements is random, and the order in which they are returned is *not*
* random. For example, choosing n random elements from an n-sized set will always return the
* elements in the same order.
*/
template <class InputIt, class Generator>
std::vector<InputIt> NRandomElements(InputIt inFirst, InputIt inLast, size_t n, Generator& g) {
    auto result = std::vector<InputIt>(n);

    for (auto& outIt : result) {
        outIt = inFirst++;
    }

    for (auto count = n; inFirst != inLast; ++inFirst, ++count) {
        auto index = UniformDistribution(g, 0, count);
        if (index < n) {
            result[index] = inFirst;
        }
    }

    return result;
}

/**
 * Generate a sequence of random bytes.
 */
template <class Generator>
std::vector<uint8_t> RandomBytes(size_t n, Generator& g) {
    std::vector<uint8_t> result(n);
    for (size_t i = 0; i < n; ++i) {
        result[i] = static_cast<uint8_t>(UniformDistribution(g, 0, 255));
    }

    return result;
}

} // namespace scraps
