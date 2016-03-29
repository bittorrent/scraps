#include "gtest/gtest.h"

#include "scraps/Wheel.h"

using namespace scraps;

TEST(Wheel, basicOperation) {
    Wheel<int> wheel;

    EXPECT_TRUE(wheel.empty());

    wheel.insert(8);

    EXPECT_EQ(wheel.size(), 1);
    EXPECT_EQ(wheel.selection(), 8);

    wheel.insert(10);

    EXPECT_EQ(wheel.size(), 2);
    EXPECT_EQ(wheel.selection(), 8);

    wheel.insert(11);

    EXPECT_EQ(wheel.size(), 3);
    EXPECT_EQ(wheel.selection(), 8);

    wheel.spin();

    EXPECT_EQ(wheel.size(), 3);
    EXPECT_EQ(wheel.selection(), 10);

    wheel.spin();

    EXPECT_EQ(wheel.size(), 3);
    EXPECT_EQ(wheel.selection(), 11);

    wheel.spin();

    EXPECT_EQ(wheel.size(), 3);
    EXPECT_EQ(wheel.selection(), 8);

    wheel.spin(-2);

    EXPECT_EQ(wheel.size(), 3);
    EXPECT_EQ(wheel.selection(), 10);

    wheel.spin(-1);

    EXPECT_EQ(wheel.size(), 3);
    EXPECT_EQ(wheel.selection(), 8);
}

TEST(Wheel, remove) {
    Wheel<int> wheel;

    wheel.insert(1);
    wheel.insert(2);
    wheel.insert(3);
    wheel.insert(4);
    wheel.insert(5);

    EXPECT_EQ(wheel.selection(), 1);

    EXPECT_FALSE(wheel.remove(8));
    EXPECT_TRUE(wheel.remove(2));

    EXPECT_EQ(wheel.size(), 4);
    EXPECT_EQ(wheel.selection(), 1);

    wheel.spin();
    EXPECT_EQ(wheel.selection(), 3);

    EXPECT_TRUE(wheel.remove(3));
    EXPECT_EQ(wheel.selection(), 4);
}

TEST(Wheel, clear) {
    Wheel<int> wheel;

    wheel.insert(1);
    wheel.insert(2);
    wheel.insert(3);

    EXPECT_EQ(wheel.size(), 3);
    EXPECT_FALSE(wheel.empty());

    wheel.clear();

    EXPECT_TRUE(wheel.empty());
}
