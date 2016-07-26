#include "scraps/URL.h"

#include <gtest/gtest.h>

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
