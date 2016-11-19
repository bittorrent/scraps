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

#include <scraps/net/utility.h>

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

TEST(NetUtility, IsLocal) {
    EXPECT_FALSE(IsLocal(Address::from_string("8.8.8.8")));
    EXPECT_TRUE(IsLocal(Address::from_string("127.0.0.1")));
    EXPECT_TRUE(IsLocal(Address::from_string("10.1.2.3")));
    EXPECT_TRUE(IsLocal(Address::from_string("192.168.1.2")));
    EXPECT_TRUE(IsLocal(Address::from_string("172.20.1.2")));
    EXPECT_FALSE(IsLocal(Address::from_string("172.10.1.2")));
}

TEST(NetUtility, ResolveRandomEndpoint) {
    auto first = ResolveRandomEndpoint("amazon.com", 80);
    ASSERT_TRUE(first);
    EXPECT_EQ(first->port(), 80);
    for (int i = 0; i < 20; ++i) {
        auto next = ResolveRandomEndpoint("amazon.com", 80);
        ASSERT_TRUE(next);
        EXPECT_EQ(next->port(), 80);
        if (*first != *next) {
            return;
        }
    }
    EXPECT_FALSE("we got the same endpoint every time. it should have been random");
}

TEST(NetUtility, ResolveRandomIPv4Endpoint) {
    auto first = ResolveRandomIPv4Endpoint("amazon.com", 80);
    ASSERT_TRUE(first);
    EXPECT_EQ(first->port(), 80);
    for (int i = 0; i < 20; ++i) {
        auto next = ResolveRandomIPv4Endpoint("amazon.com", 80);
        ASSERT_TRUE(next);
        EXPECT_TRUE(next->address().is_v4());
        EXPECT_EQ(next->port(), 80);
        if (*first != *next) {
            return;
        }
    }
    EXPECT_FALSE("we got the same endpoint every time. it should have been random");
}
