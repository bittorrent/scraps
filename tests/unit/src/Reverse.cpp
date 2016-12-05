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
#include "scraps/Reverse.h"

#include "gtest/gtest.h"

#include <vector>
#include <iterator>
#include <initializer_list>

using namespace scraps;

TEST(Reverse, worksWithRValues) {
    size_t count = 0;
    for (auto& e : Reverse(std::initializer_list<int>{4, 3, 2, 1})) {
        EXPECT_EQ(e, ++count);
    }

    ASSERT_EQ(count, 4);
}

TEST(Reverse, worksWithLValues) {
    size_t count = 0;
    std::vector<size_t> v{4, 3, 2, 1};
    for (auto& e : Reverse(v)) {
        EXPECT_EQ(e, ++count);
    }

    ASSERT_EQ(count, 4);
}
