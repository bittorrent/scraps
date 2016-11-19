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

#include <scraps/loggers.h>
#include <scraps/Timer.h>

#include <mutex>

using namespace std::literals;

TEST(loggers, DefaultLogPath) {
    const auto actual = scraps::FileLogger::DefaultLogPath("test");
#if SCRAPS_MACOS
    EXPECT_NE(actual.find("/Library/Logs/test/"), std::string::npos);
#elif SCRAPS_LINUX
    EXPECT_EQ("/var/log/test/", actual);
#elif SCRAPS_WINDOWS
    EXPECT_NE(actual.find("\\test\\"), std::string::npos);
#endif
}

class TestLogger : public scraps::Logger {
public:
    virtual void log(scraps::LogLevel level, std::chrono::system_clock::time_point time, const char* file, unsigned int line, const std::string& message) override {
        log(level, scraps::Formatf("%s %s:%u %s", scraps::LogLevelString(level), file, line, message));
    }

    virtual void log(scraps::LogLevel level, const std::string& message) override {
        std::lock_guard<std::mutex> l{mutex};
        messages.push_back(message);
    }

    std::mutex mutex;
    std::vector<std::string> messages;
};

TEST(RateLimitedLogger, basicUsage){
    auto dest = std::make_shared<TestLogger>();
    scraps::RateLimitedLogger logger{dest, 2, 2s, 1s};

    auto start = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() < start + 5s) {
        logger.log(scraps::LogLevel::kDebug, std::chrono::system_clock::now(), "foo.c", 1, "foo");
        std::this_thread::sleep_for(100ms); // ~10 per second
    }

    EXPECT_GT(dest->messages.size(), 0);
    EXPECT_LT(dest->messages.size(), 5 * 10);
    // First four would be allowed + 1 immediate reminder + 1 per second
    // afterwards. Others would be silenced. If this assert fails, it probably
    // means that burst messages aren't working.
    EXPECT_NEAR(dest->messages.size(), 11, 3);

    std::this_thread::sleep_for(3s); // enough time to reset

    logger.log(scraps::LogLevel::kDebug, std::chrono::system_clock::now(), "bar.c", 1, "bar");

    ASSERT_GT(dest->messages.size(), 0);
    EXPECT_EQ(dest->messages.back(), "DEBUG bar.c:1 bar"); // reset successful

    dest->messages.clear();

    auto start2 = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() < start2 + 6s) {
        logger.log(scraps::LogLevel::kDebug, std::chrono::system_clock::now(), "foo.c", 1, "foo");
        std::this_thread::sleep_for(800ms); // <2 per second
    }

    EXPECT_NEAR(dest->messages.size(), 10, 2); // no rate limiting
}

TEST(RateLimitedLogger, limitsLogMessagesByFileAndLine){
    auto dest = std::make_shared<TestLogger>();
    scraps::RateLimitedLogger logger{dest, 2, 2s, 1s};

    auto start = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() < start + 5s) {
        logger.log(scraps::LogLevel::kInfo, std::chrono::system_clock::now(), "foo.c", 1, "foo");
        logger.log(scraps::LogLevel::kInfo, std::chrono::system_clock::now(), "bar.c", 1, "foo");
        std::this_thread::sleep_for(100ms); // ~10 per second
    }

    EXPECT_GT(dest->messages.size(), 0);
    EXPECT_LT(dest->messages.size(), 5 * 10 * 2);
    // First four of each type would be allowed + 1 immediate reminder + 1 per
    // second afterwards. Others would be silenced. If this assert fails, it
    // probably means that burst messages aren't working.
    EXPECT_NEAR(dest->messages.size(), 11 * 2, 3 * 2);

    logger.log(scraps::LogLevel::kDebug, std::chrono::system_clock::now(), "bar.c", 1, "bar");
    logger.log(scraps::LogLevel::kWarning, std::chrono::system_clock::now(), "bar.c", 1, "bar");
    logger.log(scraps::LogLevel::kError, std::chrono::system_clock::now(), "bar.c", 1, "bar");
    logger.log(scraps::LogLevel::kInfo, std::chrono::system_clock::now(), "bar.c", 1, "bar");

    std::this_thread::sleep_for(3s); // enough time to reset

    logger.log(scraps::LogLevel::kInfo, std::chrono::system_clock::now(), "bar.c", 1, "bar");

    ASSERT_GT(dest->messages.size(), 0);
    EXPECT_EQ(dest->messages.back(), "INFO bar.c:1 bar"); // reset successful

    auto start2 = std::chrono::steady_clock::now();

    // overload messages from foo.c:1
    for (auto i = 0; i < 10; ++i) {
        logger.log(scraps::LogLevel::kInfo, std::chrono::system_clock::now(), "foo.c", 1, "foo");
    }

    // calm pace for bar.c:1
    while (std::chrono::steady_clock::now() < start2 + 6s) {
        logger.log(scraps::LogLevel::kInfo, std::chrono::system_clock::now(), "bar.c", 2, "bar");
        EXPECT_EQ(dest->messages.back(), "INFO bar.c:2 bar"); // reset successful
        std::this_thread::sleep_for(800ms); // <2 per second
    }
}

TEST(FilterLogger, basicUsage) {
    auto dest = std::make_shared<TestLogger>();
    scraps::FilterLogger filter{dest, [](scraps::LogLevel level, auto...){ return level == scraps::LogLevel::kDebug; }};

    filter.log(scraps::LogLevel::kDebug, std::chrono::system_clock::now(), "bar.c", 2, "bar");
    EXPECT_EQ(dest->messages.size(), 1);
    EXPECT_EQ(dest->messages.back(), "DEBUG bar.c:2 bar");
    filter.log(scraps::LogLevel::kError, std::chrono::system_clock::now(), "bar.c", 2, "bar");
    EXPECT_EQ(dest->messages.size(), 1);
}

TEST(LogRateLimitedMacros, basicUsage) {
    auto testLogger = std::make_shared<TestLogger>();
    SetLogger(testLogger);
    SetLogLevel(scraps::LogLevel::kDebug);
    auto threadFunc = []{
        scraps::SteadyTimer timer;
        timer.start();
        while (timer.elapsed() < 2s) {
            SCRAPS_LOG_RATE_LIMITED_DEBUG(200ms, "test message {}", 123);
            SCRAPS_LOGF_RATE_LIMITED_DEBUG(200ms, "test message %d", 123);
            SCRAPS_LOG_RATE_LIMITED_INFO(200ms, "test message {}", 123);
            SCRAPS_LOGF_RATE_LIMITED_INFO(200ms, "test message %d", 123);
            SCRAPS_LOG_RATE_LIMITED_WARNING(200ms, "test message {}", 123);
            SCRAPS_LOGF_RATE_LIMITED_WARNING(200ms, "test message %d", 123);
            SCRAPS_LOG_RATE_LIMITED_ERROR(200ms, "test message {}", 123);
            SCRAPS_LOGF_RATE_LIMITED_ERROR(200ms, "test message %d", 123);
        }
    };

    std::thread t1{threadFunc};
    std::thread t2{threadFunc};
    std::thread t3{threadFunc};

    t1.join();
    t2.join();
    t3.join();

    EXPECT_NEAR(testLogger->messages.size(), 80, 8);
}

TEST(LogCounter, countingAbility) {
    scraps::LogCounter counter;

    EXPECT_EQ(counter.count(scraps::LogLevel::kDebug), 0);
    EXPECT_EQ(counter.count(scraps::LogLevel::kInfo), 0);
    EXPECT_EQ(counter.count(scraps::LogLevel::kWarning), 0);
    EXPECT_EQ(counter.count(scraps::LogLevel::kError), 0);

    counter.log(scraps::LogLevel::kDebug, std::chrono::system_clock::now(), "bar.c", 2, "bar");
    EXPECT_EQ(counter.count(scraps::LogLevel::kDebug), 1);
    counter.log(scraps::LogLevel::kDebug, std::chrono::system_clock::now(), "bar.c", 2, "bar");
    EXPECT_EQ(counter.count(scraps::LogLevel::kDebug), 2);

    counter.log(scraps::LogLevel::kError, std::chrono::system_clock::now(), "bar.c", 2, "bar");
    EXPECT_EQ(counter.count(scraps::LogLevel::kError), 1);
}
