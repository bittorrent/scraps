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

    {
        auto now = std::chrono::steady_clock::now();
        auto result = UniformDistribution(gen, now, now + 10s);
        EXPECT_GE(result, now);
        EXPECT_LE(result, now + 10s);
    }
}

TEST(utility, RandomBytes) {
    std::random_device rd;
    std::mt19937 rng{rd()};
    const auto value = RandomBytes(32, rng);

    EXPECT_EQ(32, value.size());
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
