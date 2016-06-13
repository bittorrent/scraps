#include "gtest/gtest.h"

#include "scraps/curl.h"

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    scraps::InitCURLThreadSafety();
    return RUN_ALL_TESTS();
}
