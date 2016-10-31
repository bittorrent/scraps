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

#include "scraps/type-traits.h"

namespace scraps {

/**
 * Convenience macros for writing out the lengthy template definitions required
 * for this type of explicicity.
 */
#define SCRAPS_EXPLICIT_TEMPLATE_L_R(LType, RType) \
    template <typename L, typename R, EnableIfSameType<LType, L>* = nullptr, EnableIfSameType<RType, R>* = nullptr>

#define SCRAPS_EXPLICIT_TEMPLATE_L(LType) \
    template <typename L, EnableIfSameType<LType, L>* = nullptr>

/**
 * Provides operators that only work with the specified T and U with no
 * implicit conversions to T or U allowed (including derived to base). Derived
 * classes must implement the less-than (<) operator for Lhs < Rhs as well
 * as the less-than operator for Rhs < Lhs.
 */
template <typename T, typename U = T>
struct ExplicitTotallyOrdered {
    SCRAPS_EXPLICIT_TEMPLATE_L_R(T, U) constexpr friend bool operator==(const L& l, const R& r) noexcept { return !(l < r) && !(r < l); }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(T, U) constexpr friend bool operator> (const L& l, const R& r) noexcept { return   r < l; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(T, U) constexpr friend bool operator!=(const L& l, const R& r) noexcept { return !(l == r); }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(T, U) constexpr friend bool operator<=(const L& l, const R& r) noexcept { return !(l > r); }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(T, U) constexpr friend bool operator>=(const L& l, const R& r) noexcept { return !(l < r); }
};

/**
 * Provides operators that only work with the specified T and U with no
 * implicit conversions to T or U allowed (including derived to base). Derived
 * classes must implement "or" (|), "and" (&), and "xor" (^) for Lhs and Rhs.
 */
template <typename T, typename U = T>
struct ExplicitBitwiseAssignable {
    SCRAPS_EXPLICIT_TEMPLATE_L_R(T, U) constexpr friend T& operator|=(L& l, const R& r) noexcept { l = l | r; return l; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(T, U) constexpr friend T& operator&=(L& l, const R& r) noexcept { l = l & r; return l; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(T, U) constexpr friend T& operator^=(L& l, const R& r) noexcept { l = l ^ r; return l; }
};

/**
 * Provides operators that only work with the specified T and U with no
 * implicit conversions to T or U allowed. Derived classes must implement
 * bitwise-negation (~) and bitwise-and (&) for Lhs and Rhs;
 */
template <typename ConvertedType, typename T, typename U>
struct ExplicitBitwise {
    SCRAPS_EXPLICIT_TEMPLATE_L_R(T, U) constexpr friend ConvertedType operator|(const L& l, const R& r) noexcept { return ~(~l & ~r); }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(T, U) constexpr friend ConvertedType operator^(const L& l, const R& r) noexcept { return (l | r) & ~(l & r); }
};

/**
 * Provides operators that only work with the specified T with no implicit
 * conversions to T allowed (including derived to base). Derived classes must
 * implement left-shift (<<), and right-shift (>>) for Lhs and Rhs.
 */
template <typename T>
struct ExplicitBitwiseShiftAssignable {
    SCRAPS_EXPLICIT_TEMPLATE_L(T) constexpr friend T& operator<<=(L& l, int shift) noexcept { l = l << shift; return l; };
    SCRAPS_EXPLICIT_TEMPLATE_L(T) constexpr friend T& operator>>=(L& l, int shift) noexcept { l = l >> shift; return l; };
};

} // namespace scraps
