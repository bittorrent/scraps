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
#include "scraps/bitfield.h"

#include <gtest/gtest.h>

using namespace scraps;

TEST(bitfield, BitIterator) {
    const char* test = "abc";
    bool expected[]  = {0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1};
    size_t counted = 0;
    for (auto it = BitIterator::Begin(test, strlen(test)); it != BitIterator::End(test, strlen(test));
         ++counted, ++it) {
        EXPECT_EQ(*it, expected[counted]);
    }
    EXPECT_EQ(counted, strlen(test) * 8);
};

TEST(bitfield, EliasOmegaEncode) {
    auto code       = EliasOmegaEncode(1000000);
    bool expected[] = {1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0};
    EXPECT_EQ(code.size(), sizeof(expected));
    EXPECT_TRUE(std::equal(code.begin(), code.end(), expected));
};

TEST(bitfield, EliasOmegaDecode) {
    bool code[] = {1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0};
    EXPECT_EQ(EliasOmegaDecode(code), 1000000);
};

TEST(bitfield, BitfieldEncode) {
    bool bitfield[] = {1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1};
    auto encoded = BitfieldEncode(bitfield);
    EXPECT_EQ(encoded, "\xb4\x55\x07");
};

TEST(bitfield, BitfieldDecode) {
    std::string encoded("\xb4\x55\x07");
    auto bitfield   = BitfieldDecode(encoded.data(), encoded.size());
    bool expected[] = {1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1};
    EXPECT_EQ(bitfield.size(), sizeof(expected));
    EXPECT_TRUE(std::equal(bitfield.begin(), bitfield.end(), expected));
};
