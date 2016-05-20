#pragma once

#include "scraps/config.h"

#include <chrono>
#include <thread>

namespace scraps {

template<class Rep, class Period>
constexpr typename std::enable_if<std::chrono::duration<Rep, Period>::min() < std::chrono::duration<Rep, Period>::zero(), std::chrono::duration<Rep, Period>>::type
abs(std::chrono::duration<Rep, Period> d) {
    return d >= d.zero() ? d : -d;
}

template<typename Rep, typename Period>
std::chrono::steady_clock::duration TimedSleep(const std::chrono::duration<Rep, Period>& d) {
    auto now = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(d);
    return std::chrono::steady_clock::now() - now;
}

template<typename Rep, typename Period>
auto ToMilliseconds(std::chrono::duration<Rep, Period> d) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}

/**
* Convert from microseconds to a timeval.
*/
timeval ToTimeval(const std::chrono::microseconds& value);

} // namespace scraps
