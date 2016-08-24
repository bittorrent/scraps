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
#include "scraps/Timer.h"
#include "scraps/thread.h"

#include <gtest/gtest.h>

using namespace scraps;

TEST(HighResTimer, basicOperation) {
    HighResTimer timer;

    EXPECT_TRUE(timer.elapsed() == HighResTimer::duration::zero());

    EXPECT_TRUE(timer.stopped());

    timer.start();

    EXPECT_TRUE(!timer.stopped());

    std::this_thread::sleep_for(std::chrono::milliseconds{10});

    EXPECT_TRUE(timer.elapsed() > std::chrono::milliseconds{5} && timer.elapsed() < std::chrono::milliseconds{100});

    EXPECT_TRUE(!timer.stopped());

    timer.stop();

    EXPECT_TRUE(timer.stopped());

    auto elapsed = timer.elapsed();

    std::this_thread::sleep_for(std::chrono::milliseconds{10});

    EXPECT_TRUE(timer.elapsed() == elapsed);

    timer.reset();

    EXPECT_TRUE(timer.stopped());

    EXPECT_TRUE(timer.elapsed() == HighResTimer::duration::zero());
}
