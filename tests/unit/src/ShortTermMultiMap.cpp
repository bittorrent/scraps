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

#include <scraps/ShortTermMultiMap.h>

#include <thread>
#include <chrono>

using namespace scraps;

TEST(ShortTermMultiMap, basicOperation) {
    ShortTermMultiMap<int, std::string> map;
    map.add(2, "test", std::chrono::milliseconds(500));
    map.add(std::unordered_set<int>{{3, 4}}, "multi", std::chrono::milliseconds(500));

    EXPECT_TRUE(map.get(1).empty());

    auto values = map.get(2);
    EXPECT_EQ(values.size(), 1);
    EXPECT_EQ(*values.begin(), "test");

    auto all = map.get();
    EXPECT_EQ(all.size(), 3);
    EXPECT_EQ(*all[2].begin(), "test");

    auto values2 = map.get(3);
    EXPECT_EQ(values2.size(), 1);
    EXPECT_EQ(*values2.begin(), "multi");

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    EXPECT_TRUE(map.get(2).empty());
    EXPECT_TRUE(map.get().empty());
}

TEST(ShortTermMultiMap, types) {
    ShortTermMultiMap<int, int> map;
    (void)map.get();
}

TEST(ShortTermMultiMap, threadSafety) {
    ShortTermMultiMap<int, int> map;
    std::thread writer{[&]{
        for (int i = 0; i < 100000; ++i) {
            map.add(i, i, 10ms);
        }
    }};
    std::thread reader{[&]{
        for (int i = 0; i < 1000; ++i) {
            map.get();
        }
    }};

    writer.join();
    reader.join();
}
