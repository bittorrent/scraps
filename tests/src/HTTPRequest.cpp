#include "gtest/gtest.h"

#include "scraps/HTTPRequest.h"

using namespace scraps;

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
