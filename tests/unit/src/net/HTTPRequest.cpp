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
#include "scraps/net/HTTPRequest.h"
#include "scraps/net/utility.h"

#include <gtest/gtest.h>

#include <json.hpp>

using namespace scraps;
using namespace scraps::net;
using json = nlohmann::json;

TEST(HTTPRequest, basicGET) {
    HTTPRequest request;
    request.disablePeerVerification();
    request.initiate("https://httpbin.org/get?foo=bar");
    request.wait();

    ASSERT_TRUE(request.isComplete());
    ASSERT_FALSE(request.error());

    ASSERT_EQ(200, request.responseStatus());
    ASSERT_EQ(request.responseHeaders("content-type")[0], "application/json");

    auto body = json::parse(request.responseBody());
    EXPECT_EQ(body["args"]["foo"], "bar");
}

TEST(HTTPRequest, basicPOST) {
    HTTPRequest request("http://httpbin.org/post", "foo=bar");
    request.wait();

    ASSERT_TRUE(request.isComplete());
    ASSERT_FALSE(request.error());

    ASSERT_EQ(200, request.responseStatus());
    ASSERT_EQ(request.responseHeaders("content-type")[0], "application/json");

    auto body = json::parse(request.responseBody());
    EXPECT_EQ(body["form"]["foo"], "bar");
}

TEST(HTTPRequest, nonExistentHost) {
    auto host = "thishostshouldntexist09182309812098kjnbsdjbiuo10alamsms.com";
    if (!Resolve(host).empty()) {
        printf("*** Your ISP is hijacking your DNS queries! ***\n");
        printf("This test will fail. You should probably change your DNS servers.\n");
    }

    HTTPRequest request(host);
    request.wait();

    ASSERT_FALSE(request.isComplete());
    ASSERT_TRUE(request.error());
}

TEST(HTTPRequest, wrongPort) {
    HTTPRequest request("http://httpbin.org:9876");
    request.wait(); // we should expect to timeout while waiting

    ASSERT_FALSE(request.isComplete());
    ASSERT_TRUE(request.error());
}

TEST(HTTPRequest, abruptDestruction) {
    HTTPRequest request("http://httpbin.org:9876");
    // destroy the request without waiting
    // this is just a test to make sure we don't leave any threads running or anything like that
}
