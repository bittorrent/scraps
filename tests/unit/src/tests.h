#include "gtest/gtest.h"

/**
 * These macros allow us to use static asserts while still tracking code coverage.
 */

#define STATIC_ASSERT_EQ(a, b) \
    static_assert(a == b, ""); \
    ASSERT_EQ(a, b)

#define STATIC_ASSERT_NE(a, b) \
    static_assert(a != b, ""); \
    ASSERT_NE(a, b)

