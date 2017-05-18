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

#if _LIBCPP_VERSION >= 5000

#include <cstddef>

namespace stdts {
    using std::byte;
}

#else // _LIBCPP_VERSION < 5000

#include <type_traits>

// What follows is directly from LLVM's libc++. For full credits associated to
// the LLVM project, see CREDITS.TXT in the root of their repo. Some
// modifications have been made to allow it to compile.

// Copyright (c) 2009-2014 by the contributors listed in CREDITS.TXT
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// from
// libcxx/include/cstddef

namespace stdts  // purposefully not versioned
{
enum class byte : unsigned char {};

constexpr byte& operator|=(byte& __lhs, byte __rhs) noexcept
{ return __lhs = byte(static_cast<unsigned char>(__lhs) | static_cast<unsigned char>(__rhs)); }
constexpr byte  operator| (byte  __lhs, byte __rhs) noexcept
{ return         byte(static_cast<unsigned char>(__lhs) | static_cast<unsigned char>(__rhs)); }

constexpr byte& operator&=(byte& __lhs, byte __rhs) noexcept
{ return __lhs = byte(static_cast<unsigned char>(__lhs) & static_cast<unsigned char>(__rhs)); }
constexpr byte  operator& (byte  __lhs, byte __rhs) noexcept
{ return         byte(static_cast<unsigned char>(__lhs) & static_cast<unsigned char>(__rhs)); }

constexpr byte& operator^=(byte& __lhs, byte __rhs) noexcept
{ return __lhs = byte(static_cast<unsigned char>(__lhs) ^ static_cast<unsigned char>(__rhs)); }
constexpr byte  operator^ (byte  __lhs, byte __rhs) noexcept
{ return         byte(static_cast<unsigned char>(__lhs) ^ static_cast<unsigned char>(__rhs)); }

constexpr byte  operator~ (byte __b) noexcept
{ return  byte(~static_cast<unsigned char>(__b)); }

}

// libcxx/include/type_traits

// std::byte
namespace stdts  // purposefully not versioned
{
template <class _Integer>
  constexpr typename std::enable_if<std::is_integral<_Integer>::value, byte>::type &
  operator<<=(byte& __lhs, _Integer __shift) noexcept
  { return __lhs = byte(static_cast<unsigned char>(__lhs) << __shift); }

template <class _Integer>
  constexpr typename std::enable_if<std::is_integral<_Integer>::value, byte>::type
  operator<< (byte  __lhs, _Integer __shift) noexcept
  { return         byte(static_cast<unsigned char>(__lhs) << __shift); }

template <class _Integer>
  constexpr typename std::enable_if<std::is_integral<_Integer>::value, byte>::type &
  operator>>=(byte& __lhs, _Integer __shift) noexcept
  { return __lhs = byte(static_cast<unsigned char>(__lhs) >> __shift); }

template <class _Integer>
  constexpr typename std::enable_if<std::is_integral<_Integer>::value, byte>::type
  operator>> (byte  __lhs, _Integer __shift) noexcept
  { return         byte(static_cast<unsigned char>(__lhs) >> __shift); }

template <class _Integer>
  constexpr typename std::enable_if<std::is_integral<_Integer>::value, _Integer>::type
  to_integer(byte __b) noexcept { return _Integer(__b); }

}

#endif // _LIBCPP_VERSION < 5000
