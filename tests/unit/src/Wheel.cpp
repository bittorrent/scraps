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
#include "scraps/Wheel.h"

#include <gtest/gtest.h>

using namespace scraps;

TEST(Wheel, basicOperation) {
    Wheel<int> wheel;

    EXPECT_TRUE(wheel.empty());

    wheel.insert(8);

    EXPECT_EQ(wheel.size(), 1);
    EXPECT_EQ(wheel.selection(), 8);

    wheel.insert(10);

    EXPECT_EQ(wheel.size(), 2);
    EXPECT_EQ(wheel.selection(), 8);

    wheel.insert(11);

    EXPECT_EQ(wheel.size(), 3);
    EXPECT_EQ(wheel.selection(), 8);

    wheel.spin();

    EXPECT_EQ(wheel.size(), 3);
    EXPECT_EQ(wheel.selection(), 10);

    wheel.spin();

    EXPECT_EQ(wheel.size(), 3);
    EXPECT_EQ(wheel.selection(), 11);

    wheel.spin();

    EXPECT_EQ(wheel.size(), 3);
    EXPECT_EQ(wheel.selection(), 8);

    wheel.spin(-2);

    EXPECT_EQ(wheel.size(), 3);
    EXPECT_EQ(wheel.selection(), 10);

    wheel.spin(-1);

    EXPECT_EQ(wheel.size(), 3);
    EXPECT_EQ(wheel.selection(), 8);
}

TEST(Wheel, remove) {
    Wheel<int> wheel;

    wheel.insert(1);
    wheel.insert(2);
    wheel.insert(3);
    wheel.insert(4);
    wheel.insert(5);

    EXPECT_EQ(wheel.selection(), 1);

    EXPECT_FALSE(wheel.remove(8));
    EXPECT_TRUE(wheel.remove(2));

    EXPECT_EQ(wheel.size(), 4);
    EXPECT_EQ(wheel.selection(), 1);

    wheel.spin();
    EXPECT_EQ(wheel.selection(), 3);

    EXPECT_TRUE(wheel.remove(3));
    EXPECT_EQ(wheel.selection(), 4);
}

TEST(Wheel, clear) {
    Wheel<int> wheel;

    wheel.insert(1);
    wheel.insert(2);
    wheel.insert(3);

    EXPECT_EQ(wheel.size(), 3);
    EXPECT_FALSE(wheel.empty());

    wheel.clear();

    EXPECT_TRUE(wheel.empty());
}
