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
