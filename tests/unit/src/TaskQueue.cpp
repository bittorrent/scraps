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

#include <scraps/TaskQueue.h>

using namespace scraps;

TEST(TaskQueue,  basicFunctionality) {
    TaskQueue scheduler;

    auto argFunc = [](int x, float y, double z){};

    scheduler.async(argFunc, 1, 2, 3);
    scheduler.async(argFunc, 1, 2.0f, 3.0);

    bool executed = false;
    scheduler.async([&](bool b){ executed = b; }, true);

    EXPECT_EQ(executed, false);

    scheduler.run();

    EXPECT_EQ(executed, true);

    executed = false;
    scheduler.asyncAfter(50ms, [&]{ executed = true; });

    scheduler.run();

    EXPECT_EQ(executed, false);

    std::this_thread::sleep_for(100ms);

    EXPECT_EQ(executed, false);

    scheduler.run();

    EXPECT_EQ(executed, true);
}
