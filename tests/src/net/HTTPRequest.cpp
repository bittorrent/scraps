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

#include <gtest/gtest.h>

using namespace scraps;
using namespace scraps::net;

TEST(HTTPRequest, basicGET) {
    HTTPRequest request("http://www.google.com");
    request.wait();

    ASSERT_TRUE(request.isComplete());
    ASSERT_FALSE(request.error());

    ASSERT_EQ(200, request.responseStatus());

    std::string body = request.responseBody();
    ASSERT_EQ(0, body.find("<!doctype html>"));
    ASSERT_EQ(body.size() - 7, body.find("</html>"));
}

TEST(HTTPRequest, nonExistentHost) {
    HTTPRequest request("http://thishostshouldntexist09182309812098kjnbsdjbiuo10alamsms.com");
    request.wait();

    ASSERT_FALSE(request.isComplete());
    ASSERT_TRUE(request.error());
}

TEST(HTTPRequest, wrongPort) {
    HTTPRequest request("http://www.google.com:9876");
    request.wait(); // we should expect to timeout while waiting

    ASSERT_FALSE(request.isComplete());
    ASSERT_TRUE(request.error());
}

TEST(HTTPRequest, abruptDestruction) {
    HTTPRequest request("http://www.google.com:9876");
    // destroy the request without waiting
    // this is just a test to make sure we don't leave any threads running or anything like that
}
