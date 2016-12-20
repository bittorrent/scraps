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

#include <vector>
#include <random>
#include <chrono>
#include <cstdint>
#include <type_traits>

namespace scraps {

/**
* Produces an integer value evenly distributed across a range
*/
template <typename T, typename UniformRandomBitGenerator>
std::enable_if_t<std::is_integral<T>::value, T>
UniformDistribution(T min, T max, UniformRandomBitGenerator&& g) {
    return std::uniform_int_distribution<T>(min, max)(g);
}

/**
* Produces a real value evenly distributed across a range
*/
template <typename T, typename UniformRandomBitGenerator>
std::enable_if_t<std::is_floating_point<T>::value, T>
UniformDistribution(T min, T max, UniformRandomBitGenerator&& g) {
    return std::uniform_real_distribution<T>(min, max)(g);
}

/**
* Produces an arithmetic value evenly distributed across a range of mixed type arithmetic values
*/
template <typename T, typename U, typename UniformRandomBitGenerator>
std::enable_if_t<std::is_arithmetic<T>::value && std::is_arithmetic<U>::value, std::common_type_t<T, U>>
UniformDistribution(T min, U max, UniformRandomBitGenerator&& g) {
    using CommonType = std::common_type_t<T, U>;
    return UniformDistribution<CommonType>(static_cast<CommonType>(min), static_cast<CommonType>(max),
                                           std::forward<UniformRandomBitGenerator>(g));
}

/**
* Produces a chrono::durartion value evenly distributed across a range
*/
template <typename Rep1, typename Period1, typename Rep2, typename Period2, typename UniformRandomBitGenerator>
auto UniformDistribution(const std::chrono::duration<Rep1, Period1>& min,
                         const std::chrono::duration<Rep2, Period2>& max,
                         UniformRandomBitGenerator&& g)
{
    using CommonType = std::common_type_t<std::chrono::duration<Rep1, Period1>,
                                          std::chrono::duration<Rep2, Period2>>;
    return CommonType{UniformDistribution(CommonType{min}.count(), CommonType{max}.count(),
                                          std::forward<UniformRandomBitGenerator>(g))};
}

/**
* Produces a chrono::time_point value evenly distributed across a range
*/
template <typename Clock, typename Duration1, typename Duration2, typename UniformRandomBitGenerator>
auto UniformDistribution(const std::chrono::time_point<Clock, Duration1>& min,
                         const std::chrono::time_point<Clock, Duration2>& max,
                         UniformRandomBitGenerator&& g)
{
    using CommonType = std::common_type_t<std::chrono::time_point<Clock, Duration1>,
                                          std::chrono::time_point<Clock, Duration2>>;
    return CommonType{UniformDistribution(min.time_since_epoch(), max.time_since_epoch(),
                                          std::forward<UniformRandomBitGenerator>(g))};
}

/**
 * Generate a sequence of random bytes.
 */
template <class UniformRandomBitGenerator>
std::vector<uint8_t> RandomBytes(size_t n, UniformRandomBitGenerator&& g) {
    std::vector<uint8_t> result(n);
    for (size_t i = 0; i < n; ++i) {
        result[i] = static_cast<uint8_t>(UniformDistribution(0, 255, g));
    }

    return result;
}

} // namespace scraps
