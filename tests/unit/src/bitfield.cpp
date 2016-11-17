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

#include <scraps/bitfield.h>

using namespace scraps;

TEST(bitfield, BitIterator) {
    const char* test = "abc";
    bool expected[]  = {0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1};
    size_t counted = 0;

    auto begin = BitIterator::Begin(test, strlen(test));
    auto end = BitIterator::End(test, strlen(test));
    for (auto it = begin; it != end; ++counted, ++it) {
        EXPECT_EQ(*it, expected[counted]);
        if (counted) {
            EXPECT_EQ(*--it, expected[counted - 1]);
            ++it;
        }
    }
    EXPECT_EQ(*--end, 1);
    EXPECT_EQ(counted, strlen(test) * 8);
};

TEST(bitfield, EliasOmegaEncode) {
    auto code1       = EliasOmegaEncode(1000000);
    bool expected1[] = {1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0};
    EXPECT_EQ(code1.size(), sizeof(expected1));
    EXPECT_TRUE(std::equal(code1.begin(), code1.end(), expected1));

    auto code2       = EliasOmegaEncode(32767);
    bool expected2[] = {1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};
    EXPECT_EQ(code2.size(), sizeof(expected2));
    EXPECT_TRUE(std::equal(code2.begin(), code2.end(), expected2));

    auto code3       = EliasOmegaEncode(1048575);
    bool expected3[] = {1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};
    EXPECT_EQ(code3.size(), sizeof(expected3));
    EXPECT_TRUE(std::equal(code3.begin(), code3.end(), expected3));
};

TEST(bitfield, EliasOmegaDecode) {
    bool code1[] = {1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0};
    EXPECT_EQ(EliasOmegaDecode(code1), 1000000u);

    bool code2[] = {1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};
    EXPECT_EQ(EliasOmegaDecode(code2), 32767u);

    bool code3[] = {1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};
    EXPECT_EQ(EliasOmegaDecode(code3), 1048575u);

    bool code4[] = {1, 1, 1, 1, 1, 1, 1, 1};
    EXPECT_EQ(EliasOmegaDecode(code4), 0u);
};

TEST(bitfield, BitfieldEncode) {
    bool bitfield1[] = {1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1};
    auto encoded1 = BitfieldEncode(bitfield1);
    EXPECT_EQ(encoded1, "\xb4\x55\x07");

    bool bitfield2[] = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    auto encoded2 = BitfieldEncode(bitfield2);
    EXPECT_EQ(encoded2, "\x51\x4A\x3F");
};

TEST(bitfield, BitfieldDecode) {
    {
        std::string encoded("\xb4\x55\x07");
        auto bitfield   = BitfieldDecode(encoded.data(), encoded.size());
        bool expected[] = {1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1};
        EXPECT_EQ(bitfield.size(), sizeof(expected));
        EXPECT_TRUE(std::equal(bitfield.begin(), bitfield.end(), expected));
    }

    {
        std::string encoded("\x51\x4A\x3F");
        auto bitfield   = BitfieldDecode(encoded.data(), encoded.size());
        bool expected[] = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
        EXPECT_EQ(bitfield.size(), sizeof(expected));
        EXPECT_TRUE(std::equal(bitfield.begin(), bitfield.end(), expected));
    }

    {
        std::string encoded("");
        auto bitfield   = BitfieldDecode(encoded.data(), encoded.size());
        EXPECT_EQ(bitfield.size(), 0);
    }

    {
        // this is not a valid bitfield
        std::string encoded("\xff");
        auto bitfield   = BitfieldDecode(encoded.data(), encoded.size());
        EXPECT_EQ(bitfield.size(), 0);
    }

    {
        // expands to an unreasonable number of bits
        std::string encoded("\x6a\x85\x74\x8f\x7f\x76\x8f\x74\x8f\x41\x13");
        auto bitfield   = BitfieldDecode(encoded.data(), encoded.size());
        EXPECT_EQ(bitfield.size(), 0);
    }
};
