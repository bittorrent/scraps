#include "stdts/any.h"

#include <gtest/gtest.h>

TEST(any, basicOperation) {
    int i = 5;
    stdts::any a{i};
    EXPECT_EQ(i, stdts::any_cast<int>(a));
    EXPECT_THROW(stdts::any_cast<bool>(a), stdts::bad_any_cast);
}
