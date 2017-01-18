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

#include <scraps/net/HTTPRequest.h>
#include <scraps/net/utility.h>

#include <json11.hpp>

#include <unordered_map>

using namespace scraps;
using namespace scraps::net;
using json = json11::Json;

TEST(HTTPRequest, basicGET) {
    HTTPRequest request;
    request.disablePeerVerification();
    request.initiate("https://httpbin.org/get?foo=bar");
    request.wait();

    ASSERT_TRUE(request.isComplete());
    ASSERT_FALSE(request.error());

    ASSERT_EQ(200, request.responseStatus());
    ASSERT_EQ(request.responseHeaders("content-type")[0], "application/json");

    std::string error;
    auto body = json::parse(request.responseBody(), error);
    ASSERT_TRUE(error.empty());
    EXPECT_EQ(body["args"]["foo"], "bar");
}

TEST(HTTPRequest, basicPOST) {
    HTTPRequest request("http://httpbin.org/post", "foo=bar");
    request.wait();

    ASSERT_TRUE(request.isComplete());
    ASSERT_FALSE(request.error());

    ASSERT_EQ(200, request.responseStatus());
    ASSERT_EQ(request.responseHeaders("content-type")[0], "application/json");

    std::string error;
    auto body = json::parse(request.responseBody(), error);
    ASSERT_TRUE(error.empty());
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

TEST(HTTPRequest, HeaderValuesFromHTTPResponse) {
    EXPECT_EQ(net::detail::HeaderValuesFromHTTPResponse({
            "Set-Cookie: PHPSESSID=foobar; path=/",
        }, "Set-Cookie"),
        (std::vector<std::string>{
            "PHPSESSID=foobar; path=/",
        })
    );
}

TEST(HTTPRequest, CookiesFromHTTPResponseHeaders) {
    EXPECT_EQ(net::detail::CookiesFromHTTPResponseHeaders({
            "Set-Cookie: PHPSESSID=foobar; path=/",
            "Set-Cookie: PHPSESSID2=foobar",
            "Set-Cookie: empty=",
            "Set-Cookie: quoted=\"asdf\"",
            "Set-Cookie: quoted2=\"asdf\"; asdf=1; zxcv=2",
        }),
        (std::unordered_map<std::string, std::pair<std::string, std::vector<std::string>>>{
            {"PHPSESSID", {"foobar", {"path=/"}}},
            {"PHPSESSID2", {"foobar", {}}},
            {"empty", {"", {}}},
            {"quoted", {"asdf", {}}},
            {"quoted2", {"asdf", {"asdf=1", "zxcv=2"}}},
        })
    );
    EXPECT_EQ(net::detail::CookiesFromHTTPResponseHeaders({
            "Set-Cookie: inva>lid=asdf",
        }),
        (std::unordered_map<std::string, std::pair<std::string, std::vector<std::string>>>{})
    );
    EXPECT_EQ(net::detail::CookiesFromHTTPResponseHeaders({
            "Set-Cookie: invalid=,",
        }),
        (std::unordered_map<std::string, std::pair<std::string, std::vector<std::string>>>{})
    );
    EXPECT_EQ(net::detail::CookiesFromHTTPResponseHeaders({
            "Set-Cookie: empty=",
            "Set-Cookie: inva>lid=asdf",
        }),
        (std::unordered_map<std::string, std::pair<std::string, std::vector<std::string>>>{})
    );
    EXPECT_EQ(net::detail::CookiesFromHTTPResponseHeaders({
            "Set-Cookie: invalid=\"asdf; path=foo\"; test",
        }),
        (std::unordered_map<std::string, std::pair<std::string, std::vector<std::string>>>{})
    );
}
