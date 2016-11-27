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

#include <scraps/tuple.h>

#include <string>
#include <utility>
#include <unordered_map>

using namespace scraps;
using namespace std::literals;

TEST(tuple, hashing) {
    std::unordered_map<std::tuple<int, int, std::string>, std::string> map{
        {{0, 1, "foo"s}, "hello"s},
        {{0, 1, "bar"s}, "world"s}
    };

    EXPECT_EQ((map[{0, 1, "foo"s}]), "hello"s);
    EXPECT_EQ((map[{0, 1, "bar"s}]), "world"s);
}
