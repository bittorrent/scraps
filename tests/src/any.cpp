#include "scraps/stdts/any.h"

#include <gtest/gtest.h>

using namespace scraps;

TEST(any, basicOperation) {
    int i = 5;
    stdts::any a{i};
    EXPECT_EQ(i, stdts::any_cast<int>(a));
    EXPECT_THROW(stdts::any_cast<bool>(a), stdts::bad_any_cast);
}
