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
#include "scraps/net/utility.h"

#include <gtest/gtest.h>

using namespace scraps;
using namespace scraps::net;

TEST(NetUtility, Resolve) {
    auto addresses = Resolve("google.com");
    EXPECT_FALSE(addresses.empty());
}

TEST(NetUtility, ResolveWithIPAddress) {
    auto addresses = Resolve("127.0.0.1");
    ASSERT_FALSE(addresses.empty());
    EXPECT_EQ(addresses[0].to_v4().to_ulong(), 0x7f000001);
}
