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
#include "TestLogger.h"

#include <scraps/log/LogCounter.h>

using namespace std::literals;

TEST(LogCounter, countingAbility) {
    scraps::log::LogCounter counter;

    EXPECT_EQ(counter.count(scraps::log::Level::kDebug), 0);
    EXPECT_EQ(counter.count(scraps::log::Level::kInfo), 0);
    EXPECT_EQ(counter.count(scraps::log::Level::kWarning), 0);
    EXPECT_EQ(counter.count(scraps::log::Level::kError), 0);

    counter.log({scraps::log::Level::kDebug, "bar.c", 2, "bar"});
    EXPECT_EQ(counter.count(scraps::log::Level::kDebug), 1);
    counter.log({scraps::log::Level::kDebug, "bar.c", 2, "bar"});
    EXPECT_EQ(counter.count(scraps::log::Level::kDebug), 2);

    counter.log({scraps::log::Level::kError, "bar.c", 2, "bar"});
    EXPECT_EQ(counter.count(scraps::log::Level::kError), 1);
}
