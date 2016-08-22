#pragma once

#include "scraps/config.h"
#include "scraps/operators.h"

#include <type_traits>

namespace scraps {

#pragma pack(push, 1)

class GenericByte;

using Byte = GenericByte;

namespace detail {

template <typename>
class StrongByteBase;

} // namespace detail

/**
 * GenericByte represents a single 8-bit byte that is explicitly convertible
 * from uint8_t. Operations done on two incomptabile byte types result in
 * GenericByte. Bitwise and comparison operators are defined, but not other
 * arithmetic operators.
 */
class GenericByte
    : public ExplicitTotallyOrdered<GenericByte, int>
    , public ExplicitTotallyOrdered<GenericByte, uint8_t>
    , public ExplicitTotallyOrdered<GenericByte, GenericByte>
    , public ExplicitTotallyOrdered<    uint8_t, GenericByte>
    , public ExplicitTotallyOrdered<        int, GenericByte>

    , public ExplicitBitwise<GenericByte, GenericByte, int>
    , public ExplicitBitwise<GenericByte, GenericByte, uint8_t>
    , public ExplicitBitwise<GenericByte, GenericByte, GenericByte>
    , public ExplicitBitwise<GenericByte,     uint8_t, GenericByte>
    , public ExplicitBitwise<GenericByte,         int, GenericByte>

    , public ExplicitBitwiseAssignable<GenericByte, GenericByte>
    , public ExplicitBitwiseAssignable<GenericByte, uint8_t>
    , public ExplicitBitwiseAssignable<GenericByte, int>

    , public ExplicitBitwiseShiftAssignable<GenericByte>
{
public:
    using ByteType = uint8_t;

    GenericByte() = default;
    constexpr explicit GenericByte(ByteType b) : _b(b) {}

    // required for ExplicitBitwise
    constexpr GenericByte operator~() const noexcept { return GenericByte{static_cast<ByteType>(~_b)}; }

    SCRAPS_EXPLICIT_TEMPLATE_L_R(        int, GenericByte) constexpr friend GenericByte operator&(      L  lhs, const R& rhs) noexcept { return GenericByte{static_cast<ByteType>(lhs    & rhs._b)}; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(   ByteType, GenericByte) constexpr friend GenericByte operator&(      L& lhs, const R& rhs) noexcept { return GenericByte{static_cast<ByteType>(lhs    & rhs._b)}; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(GenericByte, GenericByte) constexpr friend GenericByte operator&(const L& lhs, const R& rhs) noexcept { return GenericByte{static_cast<ByteType>(lhs._b & rhs._b)}; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(GenericByte, ByteType   ) constexpr friend GenericByte operator&(const L& lhs,       R  rhs) noexcept { return GenericByte{static_cast<ByteType>(lhs._b & rhs)}; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(GenericByte, int        ) constexpr friend GenericByte operator&(const L& lhs,       R  rhs) noexcept { return GenericByte{static_cast<ByteType>(lhs._b & rhs)}; }

    // required for ExplicitBitwiseShiftAssignable
    constexpr friend GenericByte operator<<(const GenericByte& lhs, ByteType rhs) noexcept { return GenericByte{static_cast<ByteType>(lhs._b << rhs)}; }
    constexpr friend GenericByte operator>>(const GenericByte& lhs, ByteType rhs) noexcept { return GenericByte{static_cast<ByteType>(lhs._b >> rhs)}; }

    // comparisons required for ExplicitTotallyOrdered
    SCRAPS_EXPLICIT_TEMPLATE_L_R(GenericByte, int        ) constexpr friend bool operator<(const L& lhs,       R  rhs) noexcept { return lhs._b < rhs; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(GenericByte, ByteType   ) constexpr friend bool operator<(const L& lhs,       R  rhs) noexcept { return lhs._b < rhs; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(GenericByte, GenericByte) constexpr friend bool operator<(const L& lhs, const R& rhs) noexcept { return lhs._b < rhs._b; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(   ByteType, GenericByte) constexpr friend bool operator<(      L  lhs, const R& rhs) noexcept { return lhs    < rhs._b; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(        int, GenericByte) constexpr friend bool operator<(      L  lhs, const R& rhs) noexcept { return lhs    < rhs._b; }

    SCRAPS_EXPLICIT_TEMPLATE_L(GenericByte) constexpr friend GenericByte  operator++(L& lhs, int) noexcept { const auto ret = lhs; ++lhs._b; return ret; }
    SCRAPS_EXPLICIT_TEMPLATE_L(GenericByte) constexpr friend GenericByte  operator--(L& lhs, int) noexcept { const auto ret = lhs; --lhs._b; return ret; }
    SCRAPS_EXPLICIT_TEMPLATE_L(GenericByte) constexpr friend GenericByte& operator++(L& lhs)      noexcept { ++lhs._b; return lhs; }
    SCRAPS_EXPLICIT_TEMPLATE_L(GenericByte) constexpr friend GenericByte& operator--(L& lhs)      noexcept { --lhs._b; return lhs; }

    // conversion
    constexpr explicit operator bool() const noexcept { return _b; }

    constexpr ByteType value() const noexcept { return _b; }

    template <typename>
    friend class detail::StrongByteBase;
protected:
    ByteType _b;
};

static_assert(std::is_pod<GenericByte>::value, "GenericByte must be a pod type.");

namespace detail {

/**
 * StrongByteBase provides all of the functionality required for different
 * opaque byte types while also providing a means to reasonably operate with
 * GenericByte. For all defined non-assignment binary operators, if one
 * component is a GenericByte, the resulting type will also be a GenericByte.
 */
template <typename Derived>
class StrongByteBase
    : public GenericByte

    , public ExplicitTotallyOrdered<    Derived, GenericByte>
    , public ExplicitTotallyOrdered<    Derived, int>
    , public ExplicitTotallyOrdered<    Derived, uint8_t>
    , public ExplicitTotallyOrdered<    Derived, Derived>
    , public ExplicitTotallyOrdered<    uint8_t, Derived>
    , public ExplicitTotallyOrdered<        int, Derived>
    , public ExplicitTotallyOrdered<GenericByte, Derived>

    , public ExplicitBitwise<GenericByte,     Derived, GenericByte>
    , public ExplicitBitwise<Derived    ,     Derived, int>
    , public ExplicitBitwise<Derived    ,     Derived, uint8_t>
    , public ExplicitBitwise<Derived    ,     Derived, Derived>
    , public ExplicitBitwise<Derived    ,     uint8_t, Derived>
    , public ExplicitBitwise<Derived    ,         int, Derived>
    , public ExplicitBitwise<GenericByte, GenericByte, Derived>

    , public ExplicitBitwiseAssignable<GenericByte, Derived>
    , public ExplicitBitwiseAssignable<Derived,     Derived>
    , public ExplicitBitwiseAssignable<Derived,     uint8_t>
    , public ExplicitBitwiseAssignable<Derived,     int>

    , public ExplicitBitwiseShiftAssignable<Derived>
{
public:
    using GenericByte::GenericByte;
    using DerivedType = Derived;

    // required for ExplicitBitwise
    constexpr Derived operator~() const noexcept { return Derived{static_cast<ByteType>(~_b)}; }

    SCRAPS_EXPLICIT_TEMPLATE_L_R(    Derived, GenericByte) constexpr friend GenericByte operator&(const L& lhs, const R& rhs) noexcept { return GenericByte{static_cast<ByteType>(lhs._b & rhs._b)}; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(    Derived, int        ) constexpr friend Derived     operator&(const L& lhs,       R  rhs) noexcept { return     Derived{static_cast<ByteType>(lhs._b & rhs   )}; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(    Derived, ByteType   ) constexpr friend Derived     operator&(const L& lhs,       R  rhs) noexcept { return     Derived{static_cast<ByteType>(lhs._b & rhs   )}; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(    Derived, Derived    ) constexpr friend Derived     operator&(const L& lhs, const R& rhs) noexcept { return     Derived{static_cast<ByteType>(lhs._b & rhs._b)}; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(   ByteType, Derived    ) constexpr friend Derived     operator&(      L  lhs, const R  rhs) noexcept { return     Derived{static_cast<ByteType>(lhs    & rhs._b)}; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(        int, Derived    ) constexpr friend Derived     operator&(      L  lhs, const R  rhs) noexcept { return     Derived{static_cast<ByteType>(lhs    & rhs._b)}; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(GenericByte, Derived    ) constexpr friend GenericByte operator&(const L& lhs, const R& rhs) noexcept { return GenericByte{static_cast<ByteType>(lhs._b & rhs._b)}; }

    // required for ExplicitBitwiseShiftAssignable
    constexpr friend Derived operator<<(const Derived& lhs, ByteType rhs) noexcept { return Derived{static_cast<ByteType>(lhs._b << rhs)}; }
    constexpr friend Derived operator>>(const Derived& lhs, ByteType rhs) noexcept { return Derived{static_cast<ByteType>(lhs._b >> rhs)}; }

    // required for ExplicitTotallyOrdered
    SCRAPS_EXPLICIT_TEMPLATE_L_R(    Derived, GenericByte) constexpr friend bool operator<(const L& lhs, const R& rhs) noexcept { return lhs._b < rhs._b; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(    Derived, int        ) constexpr friend bool operator<(const L& lhs,       R  rhs) noexcept { return lhs._b < rhs;    }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(    Derived, ByteType   ) constexpr friend bool operator<(const L& lhs,       R  rhs) noexcept { return lhs._b < rhs;    }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(    Derived, Derived    ) constexpr friend bool operator<(const L& lhs, const R& rhs) noexcept { return lhs._b < rhs._b; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(   ByteType, Derived    ) constexpr friend bool operator<(      L  lhs, const R& rhs) noexcept { return    lhs < rhs._b; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(        int, Derived    ) constexpr friend bool operator<(      L  lhs, const R& rhs) noexcept { return    lhs < rhs._b; }
    SCRAPS_EXPLICIT_TEMPLATE_L_R(GenericByte, Derived    ) constexpr friend bool operator<(const L& lhs, const R& rhs) noexcept { return lhs._b < rhs._b; }

    SCRAPS_EXPLICIT_TEMPLATE_L(Derived) constexpr friend Derived  operator++(L& lhs, int) noexcept { const auto ret = lhs; ++lhs._b; return ret; }
    SCRAPS_EXPLICIT_TEMPLATE_L(Derived) constexpr friend Derived  operator--(L& lhs, int) noexcept { const auto ret = lhs; --lhs._b; return ret; }
    SCRAPS_EXPLICIT_TEMPLATE_L(Derived) constexpr friend Derived& operator++(L& lhs) noexcept { ++lhs._b; return lhs; }
    SCRAPS_EXPLICIT_TEMPLATE_L(Derived) constexpr friend Derived& operator--(L& lhs) noexcept { --lhs._b; return lhs; }
};

} // namespace detail

/**
 * StrongByte provides the public interface for strongly typed bytes. To use the
 * class properly, instantiate a unique type.
 *
 * Example:
 *
 * enum class FooTag {};
 * using FooByte = StrongByte<FooTag>;
 */
template <typename Tag>
class StrongByte : public detail::StrongByteBase<StrongByte<Tag>> {
public:
    using detail::StrongByteBase<StrongByte<Tag>>::StrongByteBase;
};

static_assert(std::is_pod<StrongByte<void>>::value, "StrongByte must be a pod type.");

#pragma pack(pop)

} // namespace scraps

namespace std {
    template <>
    struct hash<scraps::Byte> {
        constexpr size_t operator()(const scraps::Byte& byte) const {
            return byte.value();
        }
    };

    template <typename Tag>
    struct hash<scraps::StrongByte<Tag>> {
        constexpr size_t operator()(const scraps::StrongByte<Tag>& byte) const {
            return byte.value();
        }
    };

    template<>
    struct numeric_limits<scraps::Byte> : numeric_limits<scraps::Byte::ByteType> {};
} // namespace std
