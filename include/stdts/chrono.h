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

#include <chrono>

#if __cpp_lib_chrono >= 201510

namespace stdts::chrono {
    using std::chrono::round;
    using std::chrono::floor;
    using std::chrono::ceil;
    using std::chrono::abs;
} // namespace stdts::chrono

#else

#include <type_traits>

/**
 * Taken [nearly] verbatim from http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0092r1.html
 * where the c++17 reference implementation comes from. Very little verification (or formatting) has
 * been done on the implementation.
 *
 * It has been placed in the stdts namespace for compatibility and calls have been qualified to
 * stdts::chrono to ensure correct usage with c++17 code that already include the functions.
 */

namespace stdts::chrono {

namespace detail
{

template <class T>
struct is_duration
    : public std::false_type
    {};

template <class Rep, class Period>
struct is_duration<std::chrono::duration<Rep, Period>>
    : public std::true_type
    {};

}  // namespace detail

// cppcheck-suppress syntaxError
template <class To, class Rep, class Period,
          class = std::enable_if_t<detail::is_duration<To>{}>>
constexpr
To
floor(const std::chrono::duration<Rep, Period>& d)
{
    To t = std::chrono::duration_cast<To>(d);
    if (t > d)
        --t;
    return t;
}

template <class To, class Rep, class Period,
          class = std::enable_if_t<detail::is_duration<To>{}>>
constexpr
To
ceil(const std::chrono::duration<Rep, Period>& d)
{
    To t = std::chrono::duration_cast<To>(d);
    if (t < d)
        ++t;
    return t;
}

template <class To, class Rep, class Period,
          class = std::enable_if_t<detail::is_duration<To>{}
             && !std::chrono::treat_as_floating_point<typename To::rep>{}>>
constexpr
To
round(const std::chrono::duration<Rep, Period>& d)
{
    To t0 = stdts::chrono::floor<To>(d);
    To t1 = t0 + To{1};
    auto diff0 = d - t0;
    auto diff1 = t1 - d;
    if (diff0 == diff1)
    {
        if (t0.count() & 1)
            return t1;
        return t0;
    }
    else if (diff0 < diff1)
        return t0;
    return t1;
}

template <class To, class Clock, class FromDuration,
          class = std::enable_if_t<detail::is_duration<To>{}>>
constexpr
std::chrono::time_point<Clock, To>
floor(const std::chrono::time_point<Clock, FromDuration>& tp)
{
    return std::chrono::time_point<Clock, To>{stdts::chrono::floor<To>(tp.time_since_epoch())};
}

template <class To, class Clock, class FromDuration,
          class = std::enable_if_t<detail::is_duration<To>{}>>
constexpr
std::chrono::time_point<Clock, To>
ceil(const std::chrono::time_point<Clock, FromDuration>& tp)
{
    return std::chrono::time_point<Clock, To>{stdts::chrono::ceil<To>(tp.time_since_epoch())};
}

template <class To, class Clock, class FromDuration,
          class = std::enable_if_t<detail::is_duration<To>{}
             && !std::chrono::treat_as_floating_point<typename To::rep>{}>>
constexpr
std::chrono::time_point<Clock, To>
round(const std::chrono::time_point<Clock, FromDuration>& tp)
{
    return std::chrono::time_point<Clock, To>{stdts::chrono::round<To>(tp.time_since_epoch())};
}

template <class Rep, class Period,
          class = std::enable_if_t
          <
              std::chrono::duration<Rep, Period>::min() < std::chrono::duration<Rep, Period>::zero()
          > >
constexpr
std::chrono::duration<Rep, Period>
abs(std::chrono::duration<Rep, Period> d)
{
    return d >= d.zero() ? d : -d;
}

}  // namespace stdts::chrono

#endif // __cpp_lib_chrono >= 201510
