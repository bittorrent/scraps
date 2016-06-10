#include "gtest/gtest.h"

#include "scraps/MovingAverageRateCalculator.h"

#include <chrono>

using namespace scraps;

TEST(MovingAverageRateCalculator, basicUsage) {
    MovingAverageRateCalculator<uintmax_t> rc(3);

    ASSERT_EQ(rc.calculate<std::chrono::seconds>(), stdts::nullopt);
    ASSERT_EQ(rc.currentSize(), 0);

    rc.include(std::chrono::steady_clock::time_point(1s), 1000);
    rc.include(std::chrono::steady_clock::time_point(2s), 1000);
    EXPECT_EQ(rc.calculate<std::chrono::seconds>(), stdts::nullopt); // not enough samples yet
    rc.include(std::chrono::steady_clock::time_point(3s), 1000);

    EXPECT_EQ(*rc.calculate<std::chrono::seconds>(), 1000);
    EXPECT_EQ(rc.currentSize(), 3);

    rc.include(std::chrono::steady_clock::time_point(4s), 1000);
    EXPECT_EQ(*rc.calculate<std::chrono::seconds>(), 1000);
    EXPECT_EQ(rc.currentSize(), 3);

    rc.reset();

    EXPECT_EQ(rc.calculate<std::chrono::seconds>(), stdts::nullopt);
    EXPECT_EQ(rc.currentSize(), 0);
}

TEST(MovingAverageRateCalculator, handlesVariableInput) {
    MovingAverageRateCalculator<uintmax_t> rc(4);

    rc.include(std::chrono::steady_clock::time_point(0s),  1000);
    rc.include(std::chrono::steady_clock::time_point(3s),  1000);
    rc.include(std::chrono::steady_clock::time_point(9s),  2000);
    rc.include(std::chrono::steady_clock::time_point(10s), 6000); // last value won't be used for calculations yet, only time stamps

    EXPECT_EQ(*rc.calculate<std::chrono::seconds>(), 400);
    EXPECT_EQ(rc.currentSize(), 4);

    rc.reset();

    EXPECT_EQ(rc.calculate<std::chrono::seconds>(), stdts::nullopt);
}

TEST(MovingAverageRateCalculator, handlesNonMonotonicTimepoints) {
    MovingAverageRateCalculator<uintmax_t> rc(3);

    rc.include(std::chrono::steady_clock::time_point(2s), 4000);
    rc.include(std::chrono::steady_clock::time_point(0s), 1000);
    rc.include(std::chrono::steady_clock::time_point(1s), 5000);
    EXPECT_EQ(*rc.calculate<std::chrono::seconds>(), 3000);
    EXPECT_EQ(rc.currentSize(), 3);

    rc.include(std::chrono::steady_clock::time_point(4s), 4000);
    EXPECT_EQ(*rc.calculate<std::chrono::seconds>(), 3000);
    EXPECT_EQ(rc.currentSize(), 3);

    rc.include(std::chrono::steady_clock::time_point(3s), 2000);
    EXPECT_EQ(*rc.calculate<std::chrono::seconds>(), 3000);
    EXPECT_EQ(rc.currentSize(), 3);
}

TEST(MovingAverageRateCalculator, castsDurations) {
    MovingAverageRateCalculator<uintmax_t> rc(2);

    rc.include(std::chrono::steady_clock::time_point(0s), 1000);
    rc.include(std::chrono::steady_clock::time_point(1s), 0);

    EXPECT_EQ(*rc.calculate<std::chrono::milliseconds>(), 1);
}

TEST(MovingAverageRateCalculator, handlesZeroDuration) {
    MovingAverageRateCalculator<uintmax_t> rc(2);

    rc.include(std::chrono::steady_clock::time_point(0s), 0);
    EXPECT_EQ(rc.calculate<std::chrono::milliseconds>(), stdts::nullopt);

    rc.include(std::chrono::steady_clock::time_point(1ms), 1000);
    // with second resolution, the duration will be 0
    EXPECT_EQ(*rc.calculate<std::chrono::seconds>(), 0);
}
