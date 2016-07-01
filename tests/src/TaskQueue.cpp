#include "scraps/TaskQueue.h"

#include <gtest/gtest.h>

using namespace scraps;

TEST(TaskQueue,  basicFunctionality) {
    TaskQueue scheduler;

    auto argFunc = [](int x, float y, double z){};

    scheduler.async(argFunc, 1, 2, 3);
    scheduler.async(argFunc, 1, 2.0f, 3.0);

    bool executed = false;
    scheduler.async([&](bool b){ executed = b; }, true);

    EXPECT_EQ(executed, false);

    scheduler.run();

    EXPECT_EQ(executed, true);

    executed = false;
    scheduler.asyncAfter(50ms, [&]{ executed = true; });

    scheduler.run();

    EXPECT_EQ(executed, false);

    std::this_thread::sleep_for(100ms);

    EXPECT_EQ(executed, false);

    scheduler.run();

    EXPECT_EQ(executed, true);
}
