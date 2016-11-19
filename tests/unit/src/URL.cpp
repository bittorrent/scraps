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

#include <scraps/URL.h>

using namespace scraps;

TEST(URL, http) {
    URL url{"http://www.example.com"};

    EXPECT_EQ("http://www.example.com", url.str());

    EXPECT_EQ("http", url.protocol());

    EXPECT_EQ("www.example.com", url.host());

    EXPECT_EQ(80, url.port());

    EXPECT_EQ("", url.resource());
}

TEST(URL, https) {
    URL url{"https://www.example.com"};

    EXPECT_EQ("https://www.example.com", url.str());

    EXPECT_EQ("https", url.protocol());

    EXPECT_EQ("www.example.com", url.host());

    EXPECT_EQ(443, url.port());

    EXPECT_EQ("", url.resource());
}

TEST(URL, port) {
    URL url{"https://www.example.com:123/"};

    EXPECT_EQ("https://www.example.com:123/", url.str());

    EXPECT_EQ("https", url.protocol());

    EXPECT_EQ("www.example.com", url.host());

    EXPECT_EQ(123, url.port());

    EXPECT_EQ("/", url.resource());
}

TEST(URL, resource) {
    URL url{"https://www.example.com:123/sub/path"};

    EXPECT_EQ("https://www.example.com:123/sub/path", url.str());

    EXPECT_EQ("https", url.protocol());

    EXPECT_EQ("www.example.com", url.host());

    EXPECT_EQ(123, url.port());

    EXPECT_EQ("/sub/path", url.resource());
}

TEST(URL, noAuthority) {
    URL url{"https:path/thing?asd#fragment"};

    EXPECT_EQ("https", url.protocol());
    EXPECT_EQ("path/thing?asd", url.resource());
    EXPECT_EQ("path/thing", url.path());
    EXPECT_EQ("asd", url.query());
}

TEST(URL, noAuthoritySlashPath) {
    URL url{"https:///path/thing?asd#fragment"};

    EXPECT_EQ("https", url.protocol());
    EXPECT_EQ("/path/thing?asd", url.resource());
    EXPECT_EQ("/path/thing", url.path());
    EXPECT_EQ("asd", url.query());
}

TEST(URL, queryParsing) {
    URL url{"https:path/thing?asd=1&two=dos#fragment"};

    EXPECT_EQ(URL::ParseQuery(url.query())["asd"], "1");
    EXPECT_EQ(URL::ParseQuery(url.query())["two"], "dos");
}
