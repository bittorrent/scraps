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

#include <chrono>
#include <thread>

namespace scraps {

template<class Rep, class Period>
std::chrono::duration<Rep, Period> abs(std::chrono::duration<Rep, Period> d) {
    static_assert(std::chrono::duration<Rep, Period>::min() < std::chrono::duration<Rep, Period>::zero(), "Input type cannot express negative duration");
    return d >= d.zero() ? d : -d;
}

template<typename Rep, typename Period>
auto MillisecondCount(std::chrono::duration<Rep, Period> d) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}

/**
* Convert from microseconds to a timeval.
*/
timeval ToTimeval(const std::chrono::microseconds& value);

} // namespace scraps
