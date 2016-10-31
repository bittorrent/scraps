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
#include "scraps/Cache.h"

#include <gtest/gtest.h>

using namespace scraps;

TEST(Cache, basicOperation) {
    Cache<std::string> cache;

    EXPECT_EQ(cache.get(1), nullptr);

    auto one = cache.add("one", 1);
    EXPECT_NE(one, nullptr);

    EXPECT_EQ(cache.get(2), nullptr);

    EXPECT_EQ(one, cache.get(1));

    EXPECT_EQ(cache.size(), 1);

    one.reset();

    auto two = cache.add("two", 2);

    EXPECT_EQ(*cache.get(2), "two");

    EXPECT_EQ(cache.size(), 1);

    cache.clear();

    EXPECT_EQ(cache.size(), 0);
}
