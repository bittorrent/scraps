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
#include "scraps/CumulativeRateCalculator.h"

#include <gtest/gtest.h>

#include <chrono>

using namespace scraps;

TEST(CumulativeRateCalculator, basicOperation) {
    CumulativeRateCalculator<uintmax_t> rc;

    ASSERT_EQ(rc.calculate<std::chrono::seconds>(), 0);
    ASSERT_EQ(rc.samples(), 0);

    rc.include(std::chrono::steady_clock::time_point(0s), 0);
    rc.include(std::chrono::steady_clock::time_point(1s), 1000);
    rc.include(std::chrono::steady_clock::time_point(2s), 1000);

    EXPECT_EQ(rc.calculate<std::chrono::seconds>(), 1000); // 2 seconds elapsed from 0 to 2 seconds
    EXPECT_EQ(rc.samples(), 3);

    rc.reset();

    EXPECT_EQ(rc.calculate<std::chrono::seconds>(), 0);
    EXPECT_EQ(rc.samples(), 0);
}

TEST(CumulativeRateCalculator, handlesVariableInput) {
    CumulativeRateCalculator<uintmax_t> rc;

    ASSERT_EQ(rc.calculate<std::chrono::seconds>(), 0);
    ASSERT_EQ(rc.samples(), 0);

    rc.include(std::chrono::steady_clock::time_point(0s),  0);
    rc.include(std::chrono::steady_clock::time_point(3s),  1000);
    rc.include(std::chrono::steady_clock::time_point(9s),  2000);
    rc.include(std::chrono::steady_clock::time_point(10s), 7000);

    EXPECT_EQ(rc.calculate<std::chrono::seconds>(), 1000);
    EXPECT_EQ(rc.samples(), 4);

    rc.reset();

    EXPECT_EQ(rc.calculate<std::chrono::seconds>(), 0);
}

TEST(CumulativeRateCalculator, handlesNonMonotonicTimepoints) {
    CumulativeRateCalculator<uintmax_t> rc;

    rc.include(std::chrono::steady_clock::time_point(2s), 1000);
    rc.include(std::chrono::steady_clock::time_point(0s), 0);
    rc.include(std::chrono::steady_clock::time_point(1s), 1000);

    EXPECT_EQ(rc.calculate<std::chrono::seconds>(), 1000); // 2 seconds elapsed from 0 to 2 seconds
}

TEST(CumulativeRateCalculator, castsDurations) {
    CumulativeRateCalculator<uintmax_t> rc;

    rc.include(std::chrono::steady_clock::time_point(0s), 0);
    rc.include(std::chrono::steady_clock::time_point(1s), 1000);

    EXPECT_EQ(rc.calculate<std::chrono::milliseconds>(), 1); // 2 seconds elapsed from 0 to 2 seconds
}

TEST(CumulativeRateCalculator, handlesZeroDuration) {
    CumulativeRateCalculator<uintmax_t> rc;

    rc.include(std::chrono::steady_clock::time_point(0s), 0);
    EXPECT_EQ(rc.calculate<std::chrono::milliseconds>(), 0);

    rc.include(std::chrono::steady_clock::time_point(1ms), 1000);
    // with second resolution, the duration will be 0
    EXPECT_EQ(rc.calculate<std::chrono::seconds>(), 0);
}
