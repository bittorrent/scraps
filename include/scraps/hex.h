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

#include "scraps/Byte.h"
#include "scraps/Temp.h"

#include <gsl.h>

#include <iterator>

namespace scraps {

constexpr signed char DecToHex(int c) {
    if (c >= 0 && c < 16) {
        return "0123456789abcdef"[c];
    }
    return -1;
}

constexpr signed char DecToHex(const Byte& c) {
    return DecToHex(c.value());
}

template <typename CharT>
constexpr int8_t HexToDec(CharT c) {
    if (c >= '0' && c <= '9') { return c - '0'; }
    if (c >= 'a' && c <= 'f') { return c - 'a' + 10; }
    if (c >= 'A' && c <= 'F') { return c - 'A' + 10; }
    return -1;
}

/**
 * Returns a hex string representation of the input span. The output does not
 * include an "0x" prefix.
 *
 * example: ToHex(std::array<uint8_t, 1>{0xAB}) == "AB";
 */
template <typename T, std::ptrdiff_t... BytesDimension>
std::string ToHex(const gsl::span<T, BytesDimension...> range) {
    std::string ret;

    static_assert(sizeof(T) == 1, "Input span type too large");

    ret.reserve(range.size() * 2 + 2);

    for (auto& b : range) {
        ret += DecToHex(b >> 4);
        ret += DecToHex(b & 0x0F);
    }

    return ret;
}

template <typename T, size_t N>
std::string ToHex(const std::array<T, N>& in) {
    return ToHex(gsl::span<const T, N>{in});
}

/**
 * Fills a byte range from a range of hex characters. Returns true if the input
 * range is successfully converted to the output range. A prefix of "0x" or "0X"
 * is optional.
 */
template <typename HexCharT, std::ptrdiff_t HexExtent, typename ByteType, std::ptrdiff_t... BytesDimension>
constexpr bool ToBytes(const gsl::basic_string_span<const HexCharT, HexExtent> hex, const gsl::span<ByteType, BytesDimension...> bytes) {
    auto prefixSize = 0;
    if (hex.size() > 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        prefixSize += 2;
    }
    if (hex.size() - prefixSize != bytes.size() * 2) {
        return false;
    }
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        auto hi = HexToDec(hex[i * 2 + prefixSize]);
        auto lo = HexToDec(hex[i * 2 + 1 + prefixSize]);
        if (hi < 0 || lo < 0) {
            return false;
        }
        bytes[i] = ByteType{static_cast<uint8_t>((hi << 4) | lo)};
    }
    return true;
}

template <typename CharT, typename T, size_t N>
constexpr bool ToBytes(const std::basic_string<CharT>& in, std::array<T, N>& out) {
    return ToBytes(gsl::basic_string_span<const CharT>{in}, gsl::span<T, N>{out});
}

} // namespace scraps
