#include "scraps/CircularBuffer.h"

#include <gtest/gtest.h>

using namespace scraps;

TEST(CircularBuffer, basicOperation) {
    CircularBuffer<char> buffer(15);

    EXPECT_TRUE(buffer.empty());

    buffer.push("qwertyuiop", 10);
    // |qwertyuiop-----

    EXPECT_EQ(10u, buffer.contiguousSize());

    EXPECT_EQ('q', buffer.pop_front());
    // -|wertyuiop-----

    EXPECT_EQ(9u, buffer.contiguousSize());

    EXPECT_EQ('w', buffer.pop_front());
    // --|ertyuiop-----

    buffer.push("asdfghjkl", 9);
    // hjkl|tyuiopasdfg

    EXPECT_EQ(11u, buffer.contiguousSize());

    EXPECT_EQ(0, memcmp(buffer.contiguousData(), "tyuiopasdfg", buffer.contiguousSize()));

    buffer.pop(3);
    // hjkl---|iopasdfg

    EXPECT_EQ(0, memcmp(buffer.contiguousData(), "iopasdfg", buffer.contiguousSize()));

    buffer.pop(10);
    // --|kl-----------

    EXPECT_FALSE(buffer.empty());

    EXPECT_EQ(2u, buffer.contiguousSize());

    EXPECT_EQ('k', buffer.pop_front());
    // ---|l-----------

    EXPECT_EQ('l', buffer.pop_front());
    // ----------------

    EXPECT_TRUE(buffer.empty());

    buffer.push("qwertyuiop1234567890", 20);

    std::string str(buffer.begin(), buffer.end());

    EXPECT_EQ(str, "yuiop1234567890");
}
