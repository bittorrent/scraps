#include "gtest/gtest.h"

#include "scraps/CumulativeRateCalculator.h"

#include <chrono>

using namespace scraps;

TEST(CumulativeRateCalculator, basicOperation) {
    CumulativeRateCalculator<uintmax_t> rc;

    ASSERT_EQ(rc.calculate<std::chrono::seconds>(), 0);
    ASSERT_EQ(rc.samples(), 0);

    rc.include(std::chrono::steady_clock::time_point(0s), 0);
    rc.include(std::chrono::steady_clock::time_point(1s), 1000);
    rc.include(std::chrono::steady_clock::time_point(2s), 1000);

    EXPECT_EQ(rc.calculate<std::chrono::seconds>(), 1000); // 2 seconds elapsed from 0 to 2 seconds
    EXPECT_EQ(rc.samples(), 3);

    rc.reset();

    EXPECT_EQ(rc.calculate<std::chrono::seconds>(), 0);
    EXPECT_EQ(rc.samples(), 0);
}

TEST(CumulativeRateCalculator, handlesVariableInput) {
    CumulativeRateCalculator<uintmax_t> rc;

    ASSERT_EQ(rc.calculate<std::chrono::seconds>(), 0);
    ASSERT_EQ(rc.samples(), 0);

    rc.include(std::chrono::steady_clock::time_point(0s),  0);
    rc.include(std::chrono::steady_clock::time_point(3s),  1000);
    rc.include(std::chrono::steady_clock::time_point(9s),  2000);
    rc.include(std::chrono::steady_clock::time_point(10s), 7000);

    EXPECT_EQ(rc.calculate<std::chrono::seconds>(), 1000);
    EXPECT_EQ(rc.samples(), 4);

    rc.reset();

    EXPECT_EQ(rc.calculate<std::chrono::seconds>(), 0);
}

TEST(CumulativeRateCalculator, handlesNonMonotonicTimepoints) {
    CumulativeRateCalculator<uintmax_t> rc;

    rc.include(std::chrono::steady_clock::time_point(2s), 1000);
    rc.include(std::chrono::steady_clock::time_point(0s), 0);
    rc.include(std::chrono::steady_clock::time_point(1s), 1000);

    EXPECT_EQ(rc.calculate<std::chrono::seconds>(), 1000); // 2 seconds elapsed from 0 to 2 seconds
}

TEST(CumulativeRateCalculator, castsDurations) {
    CumulativeRateCalculator<uintmax_t> rc;

    rc.include(std::chrono::steady_clock::time_point(0s), 0);
    rc.include(std::chrono::steady_clock::time_point(1s), 1000);

    EXPECT_EQ(rc.calculate<std::chrono::milliseconds>(), 1); // 2 seconds elapsed from 0 to 2 seconds
}

TEST(CumulativeRateCalculator, handlesZeroDuration) {
    CumulativeRateCalculator<uintmax_t> rc;

    rc.include(std::chrono::steady_clock::time_point(0s), 0);
    EXPECT_EQ(rc.calculate<std::chrono::milliseconds>(), 0);

    rc.include(std::chrono::steady_clock::time_point(1ms), 1000);
    // with second resolution, the duration will be 0
    EXPECT_EQ(rc.calculate<std::chrono::seconds>(), 0);
}
