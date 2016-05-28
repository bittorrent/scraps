#include "gtest/gtest.h"

#include "scraps/Version.h"

using namespace scraps;

TEST(Version, basicOperation) {
    Version version{1, 2, 3, 4};

    EXPECT_EQ("1.2.3.4", version.toString());

    EXPECT_EQ(0x0102000300000004u, version.toInteger());

    Version version2{1, 2, 3, 5};

    EXPECT_EQ(version, version);
    EXPECT_NE(version2, version);
    EXPECT_GT(version2, version);
    EXPECT_LT(version, version2);

    Version version3{1, 4, 3, 4};
    EXPECT_NE(version3, version);
    EXPECT_GT(version3, version);
    EXPECT_LT(version, version3);

    EXPECT_EQ(Version("1.2.3.4"), version);
}
