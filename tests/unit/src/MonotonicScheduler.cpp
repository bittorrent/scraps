/**
* Copyright 2016 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "gtest.h"

#include <scraps/MonotonicScheduler.h>
#include <scraps/chrono.h>

#include <vector>

using namespace scraps;

using TestDuration = std::chrono::steady_clock::duration;

// Durations are offsets from now.
void MonotonicSchedulerTests(
    TestDuration                threshold,
    std::vector<std::tuple<
        TestDuration,           // time to sleep
        TestDuration,           // schedule time point
        TestDuration,           // expected output
        bool                    // will reset
    >>                          tps
) {
    bool didReset = false;
    constexpr auto kTestScaleFactor = 5;

    MonotonicScheduler a(threshold * kTestScaleFactor, [&](auto){ didReset = true; });

    const auto start = std::chrono::steady_clock::now();
    auto now = start;

    a.mockSteadyClock([&]{ return now; });

    auto prevSleepTp = start;

    for (auto& t : tps) {
        auto sleepTarget = start + std::get<0>(t) * kTestScaleFactor;
        auto sleepDuration = sleepTarget - prevSleepTp;
        prevSleepTp = sleepTarget;
        now += sleepDuration;

        auto tp = a.schedule(start + std::get<1>(t) * kTestScaleFactor);
        EXPECT_EQ(MillisecondCount(tp - start), MillisecondCount(std::get<2>(t) * kTestScaleFactor));
        EXPECT_EQ(didReset, std::get<3>(t));
        didReset = false;
    }
}

TEST(MonotonicScheduler, basicUsage) {
    MonotonicScheduler scheduler(200ms, [](auto){});

    const auto start = std::chrono::steady_clock::now();
    auto tp = scheduler.schedule(start + 500ms);

    EXPECT_NEAR(MillisecondCount(tp - start), 0, 20);
    EXPECT_NEAR(MillisecondCount(scheduler.offset()), -500, 20);
}

TEST(MonotonicScheduler, slowStart) {
    MonotonicSchedulerTests(250ms, {
        // sleep, sched tp, out tp, reset
        {    0ms,      0ms,    0ms, false },
        {  200ms,    100ms,  200ms, false },
        {  400ms,    200ms,  400ms, false },
        {  450ms,    300ms,  500ms, false },
        {  500ms,    400ms,  600ms, false },
        {  600ms,    500ms,  700ms, false },
        {  700ms,    600ms,  800ms, false },
    });
}

TEST(MonotonicScheduler, slowStartAndDelay) {
    MonotonicSchedulerTests(200ms, {
        // sleep, sched tp, out tp, reset
        {  200ms,      0ms,  200ms, false },
        {  400ms,    100ms,  400ms, false },
        {  600ms,    200ms,  600ms, false },
        {  800ms,    300ms,  800ms, false },
        { 1000ms,    400ms, 1000ms, false },
        { 1100ms,    500ms, 1100ms, false },
        { 1200ms,    600ms, 1200ms, false },
        { 1300ms,    700ms, 1300ms, false },
        { 1400ms,    800ms, 1400ms, false },
        { 1500ms,    900ms, 1500ms, false },
    });
}

TEST(MonotonicScheduler, fastStart) {
    MonotonicSchedulerTests(300ms, {
        // sleep, sched tp, out tp, reset
        {    0ms,      0ms,    0ms, false },
        {   50ms,    100ms,  100ms, false },
        {  100ms,    200ms,  200ms, false },
        {  150ms,    300ms,  300ms, false },
        {  250ms,    400ms,  400ms, false },
        {  350ms,    500ms,  500ms, false },
        {  450ms,    600ms,  600ms, false },
        {  550ms,    700ms,  700ms, false },
        {  650ms,    800ms,  800ms, false },
        {  750ms,    900ms,  900ms, false },
    });
}

TEST(MonotonicScheduler, constantInterval) {
    MonotonicSchedulerTests(200ms, {
        // sleep, sched tp, out tp, reset
        {    0ms,      0ms,    0ms, false },
        {  100ms,    100ms,  100ms, false },
        {  200ms,    200ms,  200ms, false },
        {  300ms,    300ms,  300ms, false },
        {  400ms,    400ms,  400ms, false },
        {  500ms,    500ms,  500ms, false },
        {  600ms,    600ms,  600ms, false },
        {  700ms,    700ms,  700ms, false },
        {  800ms,    800ms,  800ms, false },
        {  900ms,    900ms,  900ms, false },
    });
}

TEST(MonotonicScheduler, constantNegativeOffset) {
    MonotonicSchedulerTests(200ms, {
        // sleep, sched tp, out tp, reset
        {    0ms,   -400ms,    0ms, false },
        {  100ms,   -300ms,  100ms, false },
        {  200ms,   -200ms,  200ms, false },
        {  300ms,   -100ms,  300ms, false },
        {  400ms,      0ms,  400ms, false },
        {  500ms,    100ms,  500ms, false },
        {  600ms,    200ms,  600ms, false },
        {  700ms,    300ms,  700ms, false },
        {  800ms,    400ms,  800ms, false },
        {  900ms,    500ms,  900ms, false },
    });
}

TEST(MonotonicScheduler, constantPositiveOffset) {
    MonotonicSchedulerTests(200ms, {
        // sleep, sched tp, out tp, reset
        {    0ms,    400ms,    0ms, false },
        {  100ms,    500ms,  100ms, false },
        {  200ms,    600ms,  200ms, false },
        {  300ms,    700ms,  300ms, false },
        {  400ms,    800ms,  400ms, false },
        {  500ms,    900ms,  500ms, false },
        {  600ms,   1000ms,  600ms, false },
        {  700ms,   1100ms,  700ms, false },
        {  800ms,   1200ms,  800ms, false },
        {  900ms,   1300ms,  900ms, false },
    });
}

TEST(MonotonicScheduler, exceedThresholdForward) {
    MonotonicSchedulerTests(200ms, {
        // sleep, sched tp, out tp, reset
        {    0ms,      0ms,    0ms, false },
        {  100ms,    100ms,  100ms, false },
        {  200ms,    200ms,  200ms, false },
        {  300ms,    300ms,  300ms, false },
        {  400ms,    400ms,  400ms, false },
        {  500ms,    500ms,  500ms, false },
        {  600ms,    900ms,  600ms, true  }, // reset
        {  700ms,   1000ms,  700ms, false },
        {  800ms,   1100ms,  800ms, false },
        {  900ms,   1200ms,  900ms, false },
    });
}

TEST(MonotonicScheduler, exceedThresholdBackward) {
    MonotonicSchedulerTests(200ms, {
        // sleep, sched tp, out tp, reset
        {    0ms,      0ms,    0ms, false },
        {  100ms,    100ms,  100ms, false },
        {  200ms,    200ms,  200ms, false },
        {  300ms,    300ms,  300ms, false },
        {  400ms,    400ms,  400ms, false },
        {  500ms,    500ms,  500ms, false },
        {  600ms,      0ms,  600ms, true  }, // reset
        {  700ms,    100ms,  700ms, false },
        {  800ms,    200ms,  800ms, false },
        {  900ms,    300ms,  900ms, false },
    });
}

TEST(MonotonicScheduler, variableButWithinThreshold) {
    MonotonicSchedulerTests(300ms, {
        // sleep, sched tp, out tp, reset
        {    0ms,      0ms,    0ms, false },
        {   20ms,     20ms,   20ms, false },
        {  100ms,    100ms,  100ms, false },
        {  300ms,    300ms,  300ms, false },
        {  350ms,    350ms,  350ms, false },
        {  500ms,    500ms,  500ms, false },
        {  700ms,    700ms,  700ms, false },
        {  750ms,    750ms,  750ms, false },
    });
}

TEST(MonotonicScheduler, scheduleForwardDoesntReset) {
    MonotonicSchedulerTests(200ms, {
        // sleep, sched tp, out tp, reset
        {    0ms,      0ms,    0ms, false },
        {  100ms,    100ms,  100ms, false },
        {  200ms,    200ms,  200ms, false }, // burst of schedule points in regular intervals in the future shouldn't cause a reset
        {  210ms,    300ms,  300ms, false },
        {  220ms,    400ms,  400ms, false },
        {  230ms,    500ms,  500ms, false },
        {  240ms,    600ms,  600ms, false },
        {  250ms,    700ms,  700ms, false },
        {  260ms,    800ms,  800ms, false },
        {  270ms,    900ms,  900ms, false },
        {  280ms,   1000ms, 1000ms, false },
        {  300ms,   1100ms, 1100ms, false },
        {  310ms,   1200ms, 1200ms, false },
        {  320ms,   1300ms, 1300ms, false },
        {  330ms,   1400ms, 1400ms, false },
    });
}

TEST(MonotonicScheduler, synchronization) {
    bool didReset = false;
    MonotonicScheduler a(300ms, [&](auto) { didReset = true; });
    MonotonicScheduler b(300ms, [&](auto) { didReset = true; });

    const auto start = std::chrono::steady_clock::now();
    auto now = start;
    a.mockSteadyClock([&]{ return now; });
    b.mockSteadyClock([&]{ return now; });

    EXPECT_FALSE(didReset);

    auto first = a.schedule(start + 500ms);
    EXPECT_NEAR(MillisecondCount(first - start), 0, 20);
    EXPECT_NEAR(MillisecondCount(a.offset()), -500, 20); // first = 100. 500 - 400 = 100
    EXPECT_FALSE(didReset);

    now += 200ms;

    b.synchronizeWith(a);
    EXPECT_EQ(a.offset(), b.offset());

    auto second = b.schedule(start + 810ms);
    EXPECT_NEAR(MillisecondCount(second - first), 310, 20);
    EXPECT_FALSE(didReset);
}
