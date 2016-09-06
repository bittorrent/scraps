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
#include <gtest/gtest.h>

#include "scraps/utility.h"

#if SCRAPS_MACOS
#import <Foundation/NSProcessInfo.h>
#endif

#include <array>

using namespace scraps;

TEST(utility, Base64Encode) {
    std::string json =
        R"({
    "event": "Signed Up",
    "properties": {
        "distinct_id": "13793",
        "token": "e3bc4100330c35722740fb8c6f5abddc",
        "Referred By": "Friend"
    }
})";

    const auto base64EncodedJSON =
        "ewogICAgImV2ZW50IjogIlNpZ25lZCBVcCIsCiAgICAicHJvcGVydGllcyI6IHsKICAgICAgICAiZGlzdGluY3RfaWQiOiAiMTM3OTMiLAogIC"
        "AgICAgICJ0b2tlbiI6ICJlM2JjNDEwMDMzMGMzNTcyMjc0MGZiOGM2ZjVhYmRkYyIsCiAgICAgICAgIlJlZmVycmVkIEJ5IjogIkZyaWVuZCIK"
        "ICAgIH0KfQ==";

    auto encoded = Base64Encode(json.data(), json.size());
    EXPECT_EQ(encoded, base64EncodedJSON);

    auto decoded = Base64Decode(encoded.data(), encoded.length());
    EXPECT_EQ(decoded, json);

    // Since base64 encoding has padding characters depending on the size of the source data, test all cases:
    for (auto i = 0; i < 4; ++i) {
        json.push_back('a');
        encoded = Base64Encode(json.data(), json.size());
        decoded = Base64Decode(encoded.data(), encoded.length());
        EXPECT_EQ(decoded, json);
    }
}

TEST(utility, Clamp) {
    EXPECT_EQ(Clamp(5l, 0, 10), 5l);
    EXPECT_EQ(Clamp(5l, 6, 10), 6l);
    EXPECT_EQ(Clamp(5l, 0, 5), 5l);
    EXPECT_EQ(Clamp(0.25, 0, 5), 0.25);
    EXPECT_EQ(Clamp(1, 0.0, 5.0), 1.0);
    EXPECT_EQ(Clamp(-0.5, -1.0, 5.0), -0.5);
}

TEST(utility, JSONEscape) { ASSERT_EQ("asd\\\\ \\\"asd\\u0009", JSONEscape("asd\\ \"asd\t")); };

TEST(utility, HexToDec) {
    static_assert(HexToDec('0') == 0, "test failed");
    static_assert(HexToDec('a') == 10, "test failed");
    static_assert(HexToDec('f') == 15, "test failed");
    static_assert(HexToDec('A') == 10, "test failed");
    static_assert(HexToDec('F') == 15, "test failed");

    static_assert(HexToDec('g') == -1, "test failed");
    static_assert(HexToDec('-') == -1, "test failed");
    static_assert(HexToDec(' ') == -1, "test failed");
}

TEST(utility, DecToHex) {
    static_assert(DecToHex(0) == '0', "test failed");
    static_assert(DecToHex(10) == 'a', "test failed");
    static_assert(DecToHex(15) == 'f', "test failed");

    static_assert(DecToHex(-1) == -1, "test failed");
    static_assert(DecToHex(17) == -1, "test failed");
    static_assert(DecToHex(20) == -1, "test failed");
}

TEST(utility, ToBytes) {
    {
        std::string str{"AB"};
        std::array<scraps::Byte, 1> expected = {{scraps::Byte{0xAB}}};
        std::array<scraps::Byte, 1> actual{};

        EXPECT_TRUE(ToBytes(str, actual));
        EXPECT_EQ(expected, actual);
    }

    {
        std::string str{"0xAB"};
        std::array<scraps::Byte, 1> expected = {{scraps::Byte{0xAB}}};
        std::array<scraps::Byte, 1> actual{};

        EXPECT_TRUE(ToBytes(str, actual));
        EXPECT_EQ(expected, actual);
    }

    {
        // differing sizes
        std::string str{"ABCDEF"};
        std::array<scraps::Byte, 2> actual{};

        EXPECT_FALSE(ToBytes(str, actual));
    }

    {
        // invalid characters
        std::string str{"hello world!"};
        std::array<scraps::Byte, 6> actual{};

        EXPECT_FALSE(ToBytes(str, actual));
    }

    {
        // zero length
        std::string str{};
        std::array<scraps::Byte, 0> expected{};
        std::array<scraps::Byte, 0> actual{};

        EXPECT_TRUE(ToBytes(str, actual));
        EXPECT_EQ(expected, actual);
    }
}

TEST(utility, ToHex) {
    std::array<scraps::Byte, 8> bytes = {
        {scraps::Byte{0x01}, scraps::Byte{0x23}, scraps::Byte{0x45}, scraps::Byte{0x67}, scraps::Byte{0x89}, scraps::Byte{0xAB}, scraps::Byte{0xCD}, scraps::Byte{0xEF}}};

    EXPECT_EQ(ToHex(bytes), "0123456789abcdef");

    uint8_t dims[3][3] = {{0x01, 0x23, 0x45}, {0x67, 0x89, 0xAB}, {0xCD, 0xEF, 0x01}};

    EXPECT_EQ(ToHex(gsl::as_span(dims)), "0123456789abcdef01");

    uint8_t(*dynDims)[3] = dims;

    EXPECT_EQ(ToHex(gsl::as_span(dynDims, 3)), "0123456789abcdef01");
}

TEST(utility, PhysicalMemory) {
#if SCRAPS_MACOS
    EXPECT_EQ(PhysicalMemory(), [NSProcessInfo processInfo].physicalMemory);
#endif
    EXPECT_GT(PhysicalMemory(), 0);
}

TEST(utility, NonatomicIteration) {
    std::vector<int> numbers = {1, 2, 3, 4};
    NonatomicIteration(numbers, [&](int x) {
        if (x == 2) {
            numbers.erase(numbers.begin() + 2);
        }
        EXPECT_NE(x, 3);
        if (x == 4) {
            numbers.push_back(5);
        }
    });
    EXPECT_EQ(numbers.size(), 4);
    EXPECT_EQ(numbers, std::vector<int>({1, 2, 4, 5}));
}

TEST(utility, Trim) {
    std::string s1(" \t\r\n  ");
    std::string s2("  \r\nc");
    std::string s3("c \t");
    std::string s4("  \rc ");

    EXPECT_EQ(gsl::to_string(Trim(gsl::string_span<>(s1))), std::string{""});
    EXPECT_EQ(gsl::to_string(Trim(gsl::string_span<>(s2))), std::string{"c"});
    EXPECT_EQ(gsl::to_string(Trim(gsl::string_span<>(s3))), std::string{"c"});
    EXPECT_EQ(gsl::to_string(Trim(gsl::string_span<>(s4))), std::string{"c"});
}

TEST(utility, URLEncode) {
    EXPECT_EQ("gro%C3%9Fp%C3%B6sna", URLEncode("großpösna"));
    EXPECT_EQ("-_.+", URLEncode("-_. "));
};

TEST(utility, URLDecode) {
    EXPECT_EQ("großpösna", URLDecode("gro%C3%9Fp%C3%B6sna"));
    EXPECT_EQ("-_. ", URLDecode("-_.+"));
};

TEST(utility, ParseAddressAndPort) {
    {
        auto result = ParseAddressAndPort("google.com:443", 80);
        EXPECT_EQ(std::get<0>(result), "google.com");
        EXPECT_EQ(std::get<1>(result), 443);
    }

    {
        auto result = ParseAddressAndPort("google.com", 80);
        EXPECT_EQ(std::get<0>(result), "google.com");
        EXPECT_EQ(std::get<1>(result), 80);
    }
};

TEST(utility, Demangle) {
    EXPECT_EQ(Demangle(typeid(scraps::GenericByte).name()), "scraps::GenericByte");
}

TEST(utility, ByteFromFile) {
    auto path = std::tmpnam(nullptr);
    FILE* f = fopen(path, "wb");
    fprintf(f, "test");
    fclose(f);
    auto _ = gsl::finally([&] { unlink(path); });

    auto bytes = BytesFromFile(path);
    ASSERT_TRUE(bytes);
    EXPECT_EQ(bytes->size(), 4);
    EXPECT_EQ(memcmp(bytes->data(), "test", std::min<size_t>(bytes->size(), 4)), 0);
}
