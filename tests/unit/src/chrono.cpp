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

#include <scraps/chrono.h>

#include <chrono>
#include <string>
#include <unordered_map>

using namespace scraps;
using namespace std::literals;

TEST(chrono, hashing) {
    std::unordered_map<std::chrono::seconds, std::string> map{
        {1s, "hello"s},
        {2s, "world"s}
    };

    EXPECT_EQ(map[1s], "hello"s);
    EXPECT_EQ(map[2s], "world"s);
}
