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

#include <scraps/SlidingBuffer.h>

using namespace scraps;

TEST(SlidingBuffer, basicOperation) {
    SlidingBuffer buffer("qwerty", 6);

    EXPECT_FALSE(buffer.empty());

    buffer.push("uiop", 4);

    EXPECT_EQ(buffer.size(), 10);
    EXPECT_EQ(0, memcmp(buffer.data(), "qwertyuiop", buffer.size()));

    buffer.pop(3);

    EXPECT_EQ(buffer.size(), 7);
    EXPECT_EQ(0, memcmp(buffer.data(), "rtyuiop", buffer.size()));

    buffer.clear();

    EXPECT_TRUE(buffer.empty());

    buffer.assign("okmijn", 6);

    EXPECT_EQ(buffer.size(), 6);
    EXPECT_EQ(0, memcmp(buffer.data(), "okmijn", buffer.size()));
}
