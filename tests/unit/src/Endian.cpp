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
#include "gtest.h"

#include <scraps/Endian.h>

using namespace scraps;

TEST(Endian, basic) {
    static_assert(Endian::kNative == Endian::kLittle, "These tests will only work if native endianness is little endian");

    static_assert(NativeToBigEndian(uint8_t(0xAA)) == uint8_t(0xAA), "");
    static_assert(LittleToBigEndian(uint8_t(0xAA)) == uint8_t(0xAA), "");
    static_assert(BigToNativeEndian(uint8_t(0xAA)) == uint8_t(0xAA), "");
    static_assert(BigToLittleEndian(uint8_t(0xAA)) == uint8_t(0xAA), "");

    static_assert(NativeToBigEndian(int8_t(0xAA)) == int8_t(0xAA), "");
    static_assert(LittleToBigEndian(int8_t(0xAA)) == int8_t(0xAA), "");
    static_assert(BigToNativeEndian(int8_t(0xAA)) == int8_t(0xAA), "");
    static_assert(BigToLittleEndian(int8_t(0xAA)) == int8_t(0xAA), "");

    static_assert(NativeToBigEndian(uint16_t(0xAABB)) == uint16_t(0xBBAA), "");
    static_assert(LittleToBigEndian(uint16_t(0xAABB)) == uint16_t(0xBBAA), "");
    static_assert(BigToNativeEndian(uint16_t(0xBBAA)) == uint16_t(0xAABB), "");
    static_assert(BigToLittleEndian(uint16_t(0xBBAA)) == uint16_t(0xAABB), "");

    static_assert(NativeToBigEndian(int16_t(0xAABB)) == int16_t(0xBBAA), "");
    static_assert(LittleToBigEndian(int16_t(0xAABB)) == int16_t(0xBBAA), "");
    static_assert(BigToNativeEndian(int16_t(0xBBAA)) == int16_t(0xAABB), "");
    static_assert(BigToLittleEndian(int16_t(0xBBAA)) == int16_t(0xAABB), "");

    static_assert(NativeToBigEndian(uint32_t(0xAABBCCDD)) == uint32_t(0xDDCCBBAA), "");
    static_assert(LittleToBigEndian(uint32_t(0xAABBCCDD)) == uint32_t(0xDDCCBBAA), "");
    static_assert(BigToNativeEndian(uint32_t(0xDDCCBBAA)) == uint32_t(0xAABBCCDD), "");
    static_assert(BigToLittleEndian(uint32_t(0xDDCCBBAA)) == uint32_t(0xAABBCCDD), "");

    static_assert(NativeToBigEndian(int32_t(0xAABBCCDD)) == int32_t(0xDDCCBBAA), "");
    static_assert(LittleToBigEndian(int32_t(0xAABBCCDD)) == int32_t(0xDDCCBBAA), "");
    static_assert(BigToNativeEndian(int32_t(0xDDCCBBAA)) == int32_t(0xAABBCCDD), "");
    static_assert(BigToLittleEndian(int32_t(0xDDCCBBAA)) == int32_t(0xAABBCCDD), "");

    static_assert(NativeToBigEndian(uint64_t(0xAABBCCDDEEFF0011)) == uint64_t(0x1100FFEEDDCCBBAA), "");
    static_assert(LittleToBigEndian(uint64_t(0xAABBCCDDEEFF0011)) == uint64_t(0x1100FFEEDDCCBBAA), "");
    static_assert(BigToNativeEndian(uint64_t(0x1100FFEEDDCCBBAA)) == uint64_t(0xAABBCCDDEEFF0011), "");
    static_assert(BigToLittleEndian(uint64_t(0x1100FFEEDDCCBBAA)) == uint64_t(0xAABBCCDDEEFF0011), "");

    static_assert(NativeToBigEndian(int64_t(0xAABBCCDDEEFF0011)) == int64_t(0x1100FFEEDDCCBBAA), "");
    static_assert(LittleToBigEndian(int64_t(0xAABBCCDDEEFF0011)) == int64_t(0x1100FFEEDDCCBBAA), "");
    static_assert(BigToNativeEndian(int64_t(0x1100FFEEDDCCBBAA)) == int64_t(0xAABBCCDDEEFF0011), "");
    static_assert(BigToLittleEndian(int64_t(0x1100FFEEDDCCBBAA)) == int64_t(0xAABBCCDDEEFF0011), "");
}
