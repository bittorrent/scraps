#include "gtest/gtest.h"

#include "scraps/URL.h"

using namespace scraps;

TEST(URL, http) {
    URL url{"http://www.example.com"};

    EXPECT_EQ("http://www.example.com", url.toString());

    EXPECT_EQ("http", url.protocol());

    EXPECT_EQ("www.example.com", url.host());

    EXPECT_EQ(80, url.port());

    EXPECT_EQ("", url.resource());
}

TEST(URL, https) {
    URL url{"https://www.example.com"};

    EXPECT_EQ("https://www.example.com", url.toString());

    EXPECT_EQ("https", url.protocol());

    EXPECT_EQ("www.example.com", url.host());

    EXPECT_EQ(443, url.port());

    EXPECT_EQ("", url.resource());
}

TEST(URL, port) {
    URL url{"https://www.example.com:123/"};

    EXPECT_EQ("https://www.example.com:123/", url.toString());

    EXPECT_EQ("https", url.protocol());

    EXPECT_EQ("www.example.com", url.host());

    EXPECT_EQ(123, url.port());

    EXPECT_EQ("/", url.resource());
}

TEST(URL, resource) {
    URL url{"https://www.example.com:123/sub/path"};

    EXPECT_EQ("https://www.example.com:123/sub/path", url.toString());

    EXPECT_EQ("https", url.protocol());

    EXPECT_EQ("www.example.com", url.host());

    EXPECT_EQ(123, url.port());

    EXPECT_EQ("/sub/path", url.resource());
}
