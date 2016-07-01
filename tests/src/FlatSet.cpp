#include "scraps/FlatSet.h"

#include <gtest/gtest.h>

using namespace scraps;

TEST(FlatSet, basicOperation) {
    FlatSet<int> set;

    EXPECT_TRUE(set.empty());

    auto it = set.insert(4);
    EXPECT_EQ(*it, 4);

    it = set.insert(4);
    EXPECT_EQ(*it, 4);

    EXPECT_EQ(set.size(), 1);
    EXPECT_EQ(set.count(4), 1);

    EXPECT_EQ(set.count(5), 0);

    it = set.insert(it, 3);
    EXPECT_EQ(it, set.begin());
    EXPECT_EQ(*it, 3);

    EXPECT_EQ(set.size(), 2);
    EXPECT_EQ(set.count(3), 1);
    EXPECT_EQ(set.count(4), 1);

    it = set.insert(set.end(), 8);
    EXPECT_EQ(*it, 8);
    EXPECT_EQ(set.size(), 3);
    EXPECT_EQ(set.count(8), 1);
    EXPECT_EQ(set.count(9), 0);

    it = set.begin();
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(*(++it), 4);
    EXPECT_EQ(*(++it), 8);

    EXPECT_FALSE(set.empty());

    set.clear();
    EXPECT_TRUE(set.empty());
}
