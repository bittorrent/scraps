#include "scraps/Endian.h"

#include <gtest/gtest.h>

using namespace scraps;

TEST(Endian, basic) {
    static_assert(Endian::kNative == Endian::kLittle, "These tests will only work if native endianness is little endian");

    static_assert(NativeToBigEndian(uint8_t(0xAA)) == uint8_t(0xAA), "");
    static_assert(BigToNativeEndian(uint8_t(0xAA)) == uint8_t(0xAA), "");

    static_assert(NativeToBigEndian(int8_t(0xAA)) == int8_t(0xAA), "");
    static_assert(BigToNativeEndian(int8_t(0xAA)) == int8_t(0xAA), "");

    static_assert(NativeToBigEndian(uint16_t(0xAABB)) == uint16_t(0xBBAA), "");
    static_assert(BigToNativeEndian(uint16_t(0xBBAA)) == uint16_t(0xAABB), "");

    static_assert(NativeToBigEndian(int16_t(0xAABB)) == int16_t(0xBBAA), "");
    static_assert(BigToNativeEndian(int16_t(0xBBAA)) == int16_t(0xAABB), "");

    static_assert(NativeToBigEndian(uint32_t(0xAABBCCDD)) == uint32_t(0xDDCCBBAA), "");
    static_assert(BigToNativeEndian(uint32_t(0xDDCCBBAA)) == uint32_t(0xAABBCCDD), "");

    static_assert(NativeToBigEndian(int32_t(0xAABBCCDD)) == int32_t(0xDDCCBBAA), "");
    static_assert(BigToNativeEndian(int32_t(0xDDCCBBAA)) == int32_t(0xAABBCCDD), "");

    static_assert(NativeToBigEndian(uint64_t(0xAABBCCDDEEFF0011)) == uint64_t(0x1100FFEEDDCCBBAA), "");
    static_assert(BigToNativeEndian(uint64_t(0x1100FFEEDDCCBBAA)) == uint64_t(0xAABBCCDDEEFF0011), "");

    static_assert(NativeToBigEndian(int64_t(0xAABBCCDDEEFF0011)) == int64_t(0x1100FFEEDDCCBBAA), "");
    static_assert(BigToNativeEndian(int64_t(0x1100FFEEDDCCBBAA)) == int64_t(0xAABBCCDDEEFF0011), "");
}
