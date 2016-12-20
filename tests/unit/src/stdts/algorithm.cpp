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

#include <stdts/algorithm.h>

#include <random>

TEST(stdts_algorithm, sample) {
    std::vector<int> set{1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> counts(9, 0);

    std::random_device rd;

    for (int i = 0; i < 1000; ++i) {
        std::default_random_engine gen(rd());

        std::vector<int> result(5);

        stdts::sample(set.begin(), set.end(), result.begin(), result.size(), gen);

        EXPECT_EQ(result.size(), 5);

        std::sort(result.begin(), result.end());

        int prev = 0;

        for (auto i : result) {
            EXPECT_NE(i, prev);
            EXPECT_GT(i, 0);
            EXPECT_LT(i, 10);
            ++counts[i - 1];
            prev = i;
        }
    }

    auto minmax = std::minmax_element(counts.begin(), counts.end());
    EXPECT_GT(*minmax.first, *minmax.second * 0.8);
}
