#include "scraps/net/curl.h"

#include <gtest/gtest.h>

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    scraps::net::InitCURLThreadSafety();
    return RUN_ALL_TESTS();
}
