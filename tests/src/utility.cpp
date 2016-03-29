#include "gtest/gtest.h"

#include "scraps/platform.h"
#include "scraps/utility.h"

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

TEST(utility, URLEncode) {
    ASSERT_EQ("gro%C3%9Fp%C3%B6sna", URLEncode("großpösna"));
    ASSERT_EQ("-_.+", URLEncode("-_. "));
};

TEST(utility, URLDecode) {
    ASSERT_EQ("großpösna", URLDecode("gro%C3%9Fp%C3%B6sna"));
    ASSERT_EQ("-_. ", URLDecode("-_.+"));
};

TEST(utility, JSONEscape) { ASSERT_EQ("asd\\\\ \\\"asd\\u0009", JSONEscape("asd\\ \"asd\t")); };

TEST(utility, UniformDistribution) {
    std::random_device rd;
    std::mt19937 gen(rd());
    {
        auto result = UniformDistribution(gen, 0, 17);
        EXPECT_GE(result, 0);
        EXPECT_LE(result, 17);

        result = UniformDistribution(gen, 7, 7);
        EXPECT_EQ(result, 7);
    }

    {
        auto result = UniformDistribution(gen, -7ms, 12ms);
        EXPECT_GE(result, -7ms);
        EXPECT_LE(result, 12ms);

        result = UniformDistribution(gen, 7ms, 7ms);
        EXPECT_EQ(result, 7ms);

        result = UniformDistribution(gen, -7ms, 12s);
        EXPECT_GE(result, -7ms);
        EXPECT_LE(result, 12s);
    }

    {
        auto result = UniformDistribution(gen, 1.0f, 123.456);
        EXPECT_GE(result, 1.0);
        EXPECT_LE(result, 123.456);
    }
}

TEST(utility, NRandomElements) {
    std::vector<int> set{1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> counts(9, 0);

    std::random_device rd;

    for (int i = 0; i < 1000; ++i) {
        std::default_random_engine gen(rd());

        auto result = NRandomElements(set.begin(), set.end(), 5, gen);

        EXPECT_EQ(result.size(), 5);

        std::sort(result.begin(), result.end());

        int prev = 0;

        for (auto& i : result) {
            EXPECT_NE(*i, prev);
            EXPECT_GT(*i, 0);
            EXPECT_LT(*i, 10);
            ++counts[*i - 1];
            prev = *i;
        }
    }

    auto minmax = std::minmax_element(counts.begin(), counts.end());
    EXPECT_GT(*minmax.first, *minmax.second * 0.8);
}

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

TEST(utility, ToHex) {
    std::array<Byte, 8> bytes = {
        {Byte{0x01}, Byte{0x23}, Byte{0x45}, Byte{0x67}, Byte{0x89}, Byte{0xAB}, Byte{0xCD}, Byte{0xEF}}};

    EXPECT_EQ(ToHex(bytes), "0123456789abcdef");

    uint8_t dims[3][3] = {{0x01, 0x23, 0x45}, {0x67, 0x89, 0xAB}, {0xCD, 0xEF, 0x01}};

    EXPECT_EQ(ToHex(gsl::as_span(dims)), "0123456789abcdef01");

    uint8_t(*dynDims)[3] = dims;

    EXPECT_EQ(ToHex(gsl::as_span(dynDims, 3)), "0123456789abcdef01");
}

TEST(utility, ToBytes) {
    {
        std::string str{"AB"};
        std::array<Byte, 1> expected = {{Byte{0xAB}}};
        std::array<Byte, 1> actual{};

        EXPECT_TRUE(ToBytes(str, actual));
        EXPECT_EQ(expected, actual);
    }

    {
        std::string str{"0xAB"};
        std::array<Byte, 1> expected = {{Byte{0xAB}}};
        std::array<Byte, 1> actual{};

        EXPECT_TRUE(ToBytes(str, actual));
        EXPECT_EQ(expected, actual);
    }

    {
        // differing sizes
        std::string str{"ABCDEF"};
        std::array<Byte, 2> actual{};

        EXPECT_FALSE(ToBytes(str, actual));
    }

    {
        // invalid characters
        std::string str{"hello world!"};
        std::array<Byte, 6> actual{};

        EXPECT_FALSE(ToBytes(str, actual));
    }

    {
        // zero length
        std::string str{};
        std::array<Byte, 0> expected{};
        std::array<Byte, 0> actual{};

        EXPECT_TRUE(ToBytes(str, actual));
        EXPECT_EQ(expected, actual);
    }
}

#if SCRAPS_OS_X
#import <Foundation/NSProcessInfo.h>

TEST(utility, PhysicalMemory) {
    EXPECT_EQ(PhysicalMemory(), [NSProcessInfo processInfo].physicalMemory);
}

#endif
