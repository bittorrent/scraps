#include "scraps/Timer.h"
#include "scraps/thread.h"

#include <gtest/gtest.h>

using namespace scraps;

TEST(HighResTimer, basicOperation) {
    HighResTimer timer;

    EXPECT_TRUE(timer.elapsed() == HighResTimer::duration::zero());

    EXPECT_TRUE(timer.stopped());

    timer.start();

    EXPECT_TRUE(!timer.stopped());

    std::this_thread::sleep_for(std::chrono::milliseconds{10});

    EXPECT_TRUE(timer.elapsed() > std::chrono::milliseconds{5} && timer.elapsed() < std::chrono::milliseconds{100});

    EXPECT_TRUE(!timer.stopped());

    timer.stop();

    EXPECT_TRUE(timer.stopped());

    auto elapsed = timer.elapsed();

    std::this_thread::sleep_for(std::chrono::milliseconds{10});

    EXPECT_TRUE(timer.elapsed() == elapsed);

    timer.reset();

    EXPECT_TRUE(timer.stopped());

    EXPECT_TRUE(timer.elapsed() == HighResTimer::duration::zero());
}
