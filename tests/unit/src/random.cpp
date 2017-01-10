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

#include <scraps/random.h>

using namespace scraps;

TEST(utility, UniformDistribution) {
    std::random_device rd;
    std::mt19937 gen(rd());
    {
        auto result = UniformDistribution(0, 17, gen);
        EXPECT_GE(result, 0);
        EXPECT_LE(result, 17);

        result = UniformDistribution(7, 7, gen);
        EXPECT_EQ(result, 7);
    }

    {
        auto result = UniformDistribution<int>(gen);
        EXPECT_LE(std::numeric_limits<int>::min(), result);
        EXPECT_LE(result, std::numeric_limits<int>::max());
    }

    {
        auto result = UniformDistribution(-7ms, 12ms, gen);
        EXPECT_GE(result, -7ms);
        EXPECT_LE(result, 12ms);

        result = UniformDistribution(7ms, 7ms, gen);
        EXPECT_EQ(result, 7ms);

        result = UniformDistribution(-7ms, 12s, gen);
        EXPECT_GE(result, -7ms);
        EXPECT_LE(result, 12s);
    }

    {
        auto result = UniformDistribution(1.0f, 123.456, gen);
        EXPECT_GE(result, 1.0);
        EXPECT_LE(result, 123.456);
    }

    {
        auto now = std::chrono::steady_clock::now();
        auto result = UniformDistribution(now, now + 10s, gen);
        EXPECT_GE(result, now);
        EXPECT_LE(result, now + 10s);
    }

    {
        auto result = UniformDistribution(1, 123.456, gen);
        EXPECT_GE(result, 1.0);
        EXPECT_LE(result, 123.456);
    }
}

TEST(utility, RandomBytes) {
    std::random_device rd;
    std::mt19937 rng{rd()};
    const auto value = RandomBytes(32, rng);

    EXPECT_EQ(32, value.size());
}
