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

#include <scraps/Continuator.h>

using namespace scraps;

TEST(Continuator, basic) {
    Continuator c;

    int tasksFinished = 0;

    for (int i = 1; i <= 10; ++i) {
        auto f = std::async(std::launch::async, [=]{
            std::this_thread::sleep_for(1100ms - 100ms * i);
            return i;
        });

        c.then(std::move(f), [&, i](auto f) {
            EXPECT_EQ(f.wait_for(0s), std::future_status::ready);
            EXPECT_EQ(f.get(), i);
            ++tasksFinished;
        });
    }

    while (tasksFinished < 10) {
        c.update();
        std::this_thread::sleep_for(10ms);
    }
}
