#include "scraps/MonotonicScheduler.h"
#include "scraps/utility.h"

#include <gtest/gtest.h>

#include <vector>

using namespace scraps;

TEST(MonotonicScheduler, constantInterval) {
    auto test = [](auto interval, auto remoteOffset) {

        MonotonicScheduler scheduler(500ms);
        std::chrono::steady_clock::time_point prev{};
        std::chrono::steady_clock::time_point tp;
        const auto start = scheduler.getTimePoint();

        for (auto i = 0ms; i < interval * 10; i += interval) {
            tp = scheduler.schedule(start + i + remoteOffset);
            EXPECT_LT(abs(tp - (start + i)), 10ms);
            EXPECT_LT(prev, tp);
            prev = tp;
        }
    };

    test(100ms,  10s);
    test(400ms, -10s);
    test(5ms,    10s);
    test(375ms, -10s);
}

TEST(MonotonicScheduler, exceedThresholdForward) {
    std::vector<std::chrono::steady_clock::duration> times = {
           0ms,
         100ms,
         200ms,
         300ms,
         800ms,
         900ms,
        1000ms,
    };

    MonotonicScheduler scheduler(200ms);
    std::chrono::steady_clock::time_point prev{};
    std::chrono::steady_clock::time_point tp;
    const auto start = scheduler.getTimePoint();

    for (auto i = 0; i < times.size(); ++i) {
        tp = scheduler.schedule(start + times[i]);
        EXPECT_LT(abs(tp - (start + (i * 100ms))), 10ms);
        EXPECT_LT(prev, tp);
        prev = tp;
    }
}

TEST(MonotonicScheduler, exceedThresholdBackward) {
    std::vector<std::chrono::steady_clock::duration> times = {
          0ms,
        100ms,
        200ms,
        300ms,
        400ms,
          0ms,
        100ms,
        200ms,
    };

    MonotonicScheduler scheduler(200ms);
    std::chrono::steady_clock::time_point prev{};
    std::chrono::steady_clock::time_point tp;
    const auto start = scheduler.getTimePoint();

    for (auto i = 0; i < times.size(); ++i) {
        tp = scheduler.schedule(start + times[i]);
        EXPECT_LT(abs(tp - (start + (i * 100ms))), 10ms);
        EXPECT_LT(prev, tp);
        prev = tp;
    }
}

TEST(MonotonicScheduler, variableButWithinThreshold) {
    std::vector<std::chrono::steady_clock::duration> times = {
          0ms,
         20ms,
        100ms,
        300ms,
        350ms,
        500ms,
        700ms,
        750ms,
    };

    MonotonicScheduler scheduler(200ms);
    std::chrono::steady_clock::time_point prev{};
    std::chrono::steady_clock::time_point tp;
    const auto start = scheduler.getTimePoint();

    for (auto i = 0; i < times.size(); ++i) {
        tp = scheduler.schedule(start + times[i]);
        EXPECT_LT(abs(tp - (start + times[i])), 10ms);
        EXPECT_LT(prev, tp);
        prev = tp;
    }
}

TEST(MonotonicScheduler, synchronization) {
    bool didReset = false;
    MonotonicScheduler a(200ms, [&](std::chrono::steady_clock::time_point x) { didReset = true; });
    MonotonicScheduler b(200ms, [&](std::chrono::steady_clock::time_point x) { didReset = true; });

    auto first = a.schedule(std::chrono::steady_clock::time_point(500ms));
    std::this_thread::sleep_for(200ms);
    b.synchronizeWith(a);
    auto second = b.schedule(std::chrono::steady_clock::time_point(1010ms));
    EXPECT_FALSE(didReset);
    EXPECT_NEAR(std::chrono::duration_cast<std::chrono::microseconds>(second - first).count(), 510000, 2);
}
