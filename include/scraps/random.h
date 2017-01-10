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
* Produces an arithmetic value evenly distributed across: 0-max (ints) or 0-1 (reals)
*/
template <class T, class RNG>
std::enable_if_t<std::is_arithmetic<T>::value, T>
UniformDistribution(RNG&& g) {
    using DistributionType = std::conditional_t<
        std::is_integral<T>::value,
        std::uniform_int_distribution<T>,
        std::uniform_real_distribution<T>
    >;
    return DistributionType{}(g);
}

/**
* Produces an arithmetic value evenly distributed across a range
*/
template <class T, class U, class RNG>
std::enable_if_t<std::is_arithmetic<T>::value && std::is_arithmetic<U>::value, std::common_type_t<T, U>>
UniformDistribution(T min, U max, RNG&& g) {
    using CommonType = std::common_type_t<T, U>;
    using DistributionType = std::conditional_t<
        std::is_integral<CommonType>::value,
        std::uniform_int_distribution<CommonType>,
        std::uniform_real_distribution<CommonType>
    >;
    return DistributionType(min, max)(g);
}

/**
* Produces a chrono::durartion value evenly distributed across a range
*/
template <class Rep1, class Period1, class Rep2, class Period2, class RNG>
auto UniformDistribution(const std::chrono::duration<Rep1, Period1>& min,
                         const std::chrono::duration<Rep2, Period2>& max,
                         RNG&& g)
{
    using CommonType = std::common_type_t<
        std::chrono::duration<Rep1, Period1>,
        std::chrono::duration<Rep2, Period2>
    >;
    return CommonType{UniformDistribution(CommonType{min}.count(), CommonType{max}.count(), g)};
}

/**
* Produces a chrono::time_point value evenly distributed across a range
*/
template <class Clock, class Duration1, class Duration2, class RNG>
auto UniformDistribution(const std::chrono::time_point<Clock, Duration1>& min,
                         const std::chrono::time_point<Clock, Duration2>& max,
                         RNG&& g)
{
    using CommonType = std::common_type_t<
        std::chrono::time_point<Clock, Duration1>,
        std::chrono::time_point<Clock, Duration2>
    >;
    return CommonType{UniformDistribution(min.time_since_epoch(), max.time_since_epoch(), g)};
}

/**
 * Generate a sequence of random bytes.
 */
template <class RNG>
std::vector<uint8_t> RandomBytes(size_t n, RNG&& g) {
    std::vector<uint8_t> result(n);
    std::uniform_int_distribution<uint8_t> dist;

    for (auto& i : result) {
        i = dist(g);
    }

    return result;
}

} // namespace scraps
