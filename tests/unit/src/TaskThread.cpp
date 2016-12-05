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
#include "scraps/TaskThread.h"

#include <gtest/gtest.h>

using namespace scraps;

// Realtime tests almost always fail with Valgrind
#ifndef VALGRIND
TEST(TaskThread, basicFunctionality) {
    TaskThread scheduler;

    auto argFunc = [](int x, float y, double z) {};

    auto f1 = scheduler.async(argFunc, 1, 2, 3);
    auto f2 = scheduler.async(argFunc, 1, 2.0f, 3.0);
    f1.wait();
    f2.wait();

    scheduler.asyncAt(std::chrono::steady_clock::now() + 5ms, argFunc, 1, 2, 3);
    scheduler.asyncAfter(5ms, argFunc, 1, 2, 3);

    std::this_thread::sleep_for(10ms);

    int first = 0;
    scheduler.asyncAfter(10ms,
                         [&] {
                             if (first == 0)
                                 first = 1;
                         });
    scheduler.asyncAfter(5ms,
                         [&] {
                             if (first == 0)
                                 first = 2;
                         });

    std::this_thread::sleep_for(20ms);

    EXPECT_EQ(first, 2);
}

TEST(TaskThread, timingFunctionality) {
    TaskThread scheduler;

    auto now = std::chrono::steady_clock::now();

    const auto threshold = 30ms;

    auto speedCheck = [=] { EXPECT_LT(std::chrono::steady_clock::now().time_since_epoch().count(), (now + threshold).time_since_epoch().count()); };

    for (int i = 0; i < 50; ++i) {
        scheduler.async(speedCheck);
    }

    now = std::chrono::steady_clock::now();

    for (auto i = 1us; i < 50us; i += 1us) {
        scheduler.asyncAt(now + i * 2, speedCheck);
        scheduler.asyncAfter(i, speedCheck);
    }

    std::this_thread::sleep_for(20ms);

    now = std::chrono::steady_clock::now();

    auto timeCheck = [&](auto duration) {
        auto d = std::chrono::steady_clock::now() - (now + duration);
        EXPECT_GT(d, -threshold);
        EXPECT_LT(d, threshold);
    };

    scheduler.asyncAfter(5ms, timeCheck, 5ms);

    scheduler.asyncAt(now + 5ms, timeCheck, 5ms);

    std::this_thread::sleep_for(50ms);
}

TEST(TaskThread, futuresFunctionality) {
    TaskThread scheduler;
    auto now = std::chrono::steady_clock::now();

    auto f = scheduler.asyncAfter(5ms, [&] { return 17; });
    EXPECT_EQ(f.valid(), true);
    EXPECT_EQ(f.get(), 17);

    const auto threshold = 20ms;

    EXPECT_LT(std::chrono::steady_clock::now(), now + threshold);
}

TEST(TaskThread, cancelAndJoin) {
    TaskThread scheduler;

    std::atomic<size_t> invocations{0};
    std::function<void()> repeat;
    repeat = [&] {
        ++invocations;
        std::this_thread::sleep_for(100ms);
        scheduler.async(repeat);
    };
    scheduler.async(repeat);
    scheduler.async(repeat);
    while (invocations < 1);
    scheduler.cancelAndJoin();
    scheduler.async(repeat);

    std::this_thread::sleep_for(300ms);
    EXPECT_EQ(invocations, 1);
}

TEST(TaskThread, scopeFunctionality) {
    {
        TaskThread scheduler;

        TaskThread::TaskScope scope;

        bool executed1 = false;
        scheduler.async(scope, [&] { executed1 = true; });

        std::this_thread::sleep_for(10ms);

        scope.endScope();

        bool executed2 = false;
        scheduler.async(scope, [&] { executed2 = true; });

        std::this_thread::sleep_for(10ms);

        EXPECT_EQ(executed1, true);
        EXPECT_EQ(executed2, false);
    }

    {
        TaskThread scheduler;

        auto scope = std::make_unique<TaskThread::TaskScope>();

        bool executed1 = false;
        scheduler.async(*scope,
                        [&] {
                            executed1 = true;
                            std::this_thread::sleep_for(100ms);
                        });

        bool executed2 = false;
        scheduler.asyncAfter(*scope, 1ms, [&] { executed2 = true; });

        std::this_thread::sleep_for(10ms); // let the first task get going, but not end yet before resetting the scope

        scope.reset(); // will block until task1 is done

        EXPECT_EQ(executed1, true);
        EXPECT_EQ(executed2, false);
    }
}

TEST(TaskThread, scopeRemoval) {
    {
        TaskThread scheduler;

        TaskThread::TaskScope scope;

        auto test     = std::make_shared<int>(7);
        auto weakTest = std::weak_ptr<int>{test};

        EXPECT_EQ((bool)weakTest.lock(), true);

        scheduler.async(scope, [test = std::move(test)]{});

        std::this_thread::sleep_for(10ms);

        scope.endScope();

        EXPECT_EQ((bool)test, false);
        EXPECT_EQ((bool)weakTest.lock(), false);
    }

    {
        TaskThread scheduler;

        TaskThread::TaskScope scope;

        auto test     = std::make_shared<int>(7);
        auto weakTest = std::weak_ptr<int>{test};

        EXPECT_EQ((bool)weakTest.lock(), true);

        scheduler.asyncAfter(scope, 5s, [test = std::move(test)]{});

        EXPECT_EQ((bool)weakTest.lock(), true);

        scope.endScope();

        EXPECT_EQ((bool)test, false);
        EXPECT_EQ((bool)weakTest.lock(), false);
    }

    {
        TaskThread scheduler;

        TaskThread::TaskScope scope;

        auto test     = std::make_shared<int>(7);
        auto weakTest = std::weak_ptr<int>{test};

        EXPECT_EQ((bool)weakTest.lock(), true);

        scheduler.asyncAfter(scope, 50ms, [test = std::move(test)]{});

        std::this_thread::sleep_for(100ms);

        EXPECT_EQ((bool)test, false);
        EXPECT_EQ((bool)weakTest.lock(), false);
    }
}
#endif // VALGRIND
