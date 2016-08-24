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

#include "scraps/Endian.h"

namespace scraps {

#pragma pack(push, 1)

template <typename T, Endian endian>
class FixedEndian {
public:
    static_assert(std::is_integral<T>::value, "FixedEndian can only be used for integers");

    constexpr FixedEndian(T value = T{}) noexcept;
    constexpr operator T() const noexcept;

private:
    T _value;
};

#pragma pack(pop)

template <typename T, Endian endian>
constexpr FixedEndian<T, endian>::FixedEndian(T value) noexcept
    : _value{detail::MaybeByteSwap(value, std::integral_constant<bool, Endian::kNative != endian>{})}
{}

template <typename T, Endian endian>
constexpr FixedEndian<T, endian>::operator T() const noexcept {
    return detail::MaybeByteSwap(_value, std::integral_constant<bool, Endian::kNative != endian>{});
}

template <typename T>
using LittleEndian = FixedEndian<T, Endian::kLittle>;

template <typename T>
using BigEndian = FixedEndian<T, Endian::kBig>;

} // namespace scraps
