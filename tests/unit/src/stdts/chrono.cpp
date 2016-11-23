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
#include "../gtest.h"

#include <stdts/chrono.h>

using namespace std::literals;

TEST(stdts_chrono_round, basics) {
    EXPECT_EQ(stdts::chrono::round<std::chrono::seconds>(std::chrono::duration<double>(4.4)), 4s);
    EXPECT_EQ(stdts::chrono::round<std::chrono::seconds>(std::chrono::duration<double>(4.6)), 5s);
    EXPECT_EQ(stdts::chrono::round<std::chrono::seconds>(std::chrono::duration<double>(-4.4)), -4s);
    EXPECT_EQ(stdts::chrono::round<std::chrono::seconds>(std::chrono::duration<double>(-4.6)), -5s);
}

TEST(stdts_chrono_abs, basics) {
    EXPECT_EQ(stdts::chrono::abs(-4s), 4s);
    EXPECT_EQ(stdts::chrono::abs(-5ms), 5ms);
    EXPECT_EQ(stdts::chrono::abs(0ms), 0ms);
}

TEST(stdts_chrono_floor, basics) {
    EXPECT_EQ(stdts::chrono::floor<std::chrono::seconds>(4500ms), 4s);
    EXPECT_EQ(stdts::chrono::floor<std::chrono::seconds>(-4500ms), -5s);
}

TEST(stdts_chrono_ceil, basics) {
    EXPECT_EQ(stdts::chrono::ceil<std::chrono::seconds>(4500ms), 5s);
    EXPECT_EQ(stdts::chrono::ceil<std::chrono::seconds>(-4500ms), -4s);
}
