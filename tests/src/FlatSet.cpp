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
#include "scraps/FlatSet.h"

#include <gtest/gtest.h>

using namespace scraps;

TEST(FlatSet, basicOperation) {
    FlatSet<int> set;

    EXPECT_TRUE(set.empty());

    auto it = set.insert(4);
    EXPECT_EQ(*it, 4);

    it = set.insert(4);
    EXPECT_EQ(*it, 4);

    EXPECT_EQ(set.size(), 1);
    EXPECT_EQ(set.count(4), 1);

    EXPECT_EQ(set.count(5), 0);

    it = set.insert(it, 3);
    EXPECT_EQ(it, set.begin());
    EXPECT_EQ(*it, 3);

    EXPECT_EQ(set.size(), 2);
    EXPECT_EQ(set.count(3), 1);
    EXPECT_EQ(set.count(4), 1);

    it = set.insert(set.end(), 8);
    EXPECT_EQ(*it, 8);
    EXPECT_EQ(set.size(), 3);
    EXPECT_EQ(set.count(8), 1);
    EXPECT_EQ(set.count(9), 0);

    it = set.begin();
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(*(++it), 4);
    EXPECT_EQ(*(++it), 8);

    EXPECT_FALSE(set.empty());

    set.clear();
    EXPECT_TRUE(set.empty());
}
