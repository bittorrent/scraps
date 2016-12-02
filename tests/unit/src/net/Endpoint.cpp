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

#include <scraps/net/Endpoint.h>
#include <scraps/net/Address.h>

using namespace scraps;

TEST(Endpoint, toFromSockAddr) {
    net::Endpoint expected{net::Address::from_string("127.0.0.1"), 8080};

    sockaddr_storage ss;
    socklen_t len;

    expected.getSockAddr(&ss, &len);
    EXPECT_EQ(len, ss.ss_family == AF_INET ? sizeof(sockaddr_in) : sizeof(sockaddr_in6));

    EXPECT_EQ(expected, net::Endpoint::FromSockaddr(reinterpret_cast<sockaddr*>(&ss), len));
}
