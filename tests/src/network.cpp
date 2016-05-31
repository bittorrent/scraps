#include "gtest/gtest.h"

#include "scraps/network.h"

using namespace scraps;

TEST(network, ResolveIPV4) {
    boost::system::error_code ec;
    auto endpoint = ResolveIPv4("google.com:80", ec);
    ASSERT_FALSE(ec);
    ASSERT_EQ(80, endpoint.port());

    ec.clear();
    endpoint = ResolveIPv4("google.com", ec);
    ASSERT_FALSE(ec);
    ASSERT_FALSE(endpoint.port());

    ec.clear();
    endpoint = ResolveIPv4("google.com:", ec);
    ASSERT_FALSE(ec);
    ASSERT_FALSE(endpoint.port());
}
