#include "gtest/gtest.h"

#include "scraps/Reverse.h"

#include <vector>
#include <iterator>
#include <initializer_list>

using namespace scraps;

TEST(Reverse, worksWithRValues) {
    size_t count = 0;
    for (auto& e : Reverse(std::initializer_list<int>{4, 3, 2, 1})) {
        EXPECT_EQ(e, ++count);
    }

    ASSERT_EQ(count, 4);
}

TEST(Reverse, worksWithLValues) {
    size_t count = 0;
    std::vector<size_t> v{4, 3, 2, 1};
    for (auto& e : Reverse(v)) {
        EXPECT_EQ(e, ++count);
    }

    ASSERT_EQ(count, 4);
}
