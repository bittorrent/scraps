#include "gtest/gtest.h"

#include "scraps/SlidingBuffer.h"

using namespace scraps;

TEST(SlidingBuffer, basicOperation) {
    SlidingBuffer buffer("qwerty", 6);

    EXPECT_FALSE(buffer.empty());

    buffer.push("uiop", 4);

    EXPECT_EQ(buffer.size(), 10);
    EXPECT_EQ(0, memcmp(buffer.data(), "qwertyuiop", buffer.size()));

    buffer.pop(3);

    EXPECT_EQ(buffer.size(), 7);
    EXPECT_EQ(0, memcmp(buffer.data(), "rtyuiop", buffer.size()));

    buffer.clear();

    EXPECT_TRUE(buffer.empty());

    buffer.assign("okmijn", 6);

    EXPECT_EQ(buffer.size(), 6);
    EXPECT_EQ(0, memcmp(buffer.data(), "okmijn", buffer.size()));
}
