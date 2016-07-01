#include "scraps/loggers.h"

#include <gtest/gtest.h>

using namespace scraps;

TEST(loggers, DefaultLogPath) {
    const auto actual = FileLogger::DefaultLogPath("test");
#if SCRAPS_MAC_OS_X
    EXPECT_NE(actual.find("/Library/Logs/test/"), std::string::npos);
#elif SCRAPS_LINUX
    EXPECT_EQ("/var/log/test/", actual);
#elif SCRAPS_WINDOWS
    EXPECT_NE(actual.find("\\test\\"), std::string::npos);
#endif
}

class TestLogger : public Logger {
public:
    virtual void log(LogLevel level, std::chrono::system_clock::time_point time, const char* file, unsigned int line, const std::string& message) override {
        log(level, Formatf("%s %s:%u %s", LogLevelString(level), file, line, message));
    }

    virtual void log(LogLevel level, const std::string& message) override {
        messages.push_back(message);
    }

    std::vector<std::string> messages;
};

TEST(RateLimitedLogger, basicUsage){
    auto dest = std::make_shared<TestLogger>();
    RateLimitedLogger logger{dest, 2, 2s, 1s};

    auto start = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() < start + 5s) {
        logger.log(LogLevel::kDebug, std::chrono::system_clock::now(), "foo.c", 1, "foo");
        std::this_thread::sleep_for(100ms); // ~10 per second
    }

    EXPECT_GT(dest->messages.size(), 0);
    EXPECT_LT(dest->messages.size(), 5 * 10);
    // First four would be allowed + 1 immediate reminder + 1 per second
    // afterwards. Others would be silenced. If this assert fails, it probably
    // means that burst messages aren't working.
    EXPECT_NEAR(dest->messages.size(), 11, 3);

    std::this_thread::sleep_for(3s); // enough time to reset

    logger.log(LogLevel::kDebug, std::chrono::system_clock::now(), "bar.c", 1, "bar");

    ASSERT_GT(dest->messages.size(), 0);
    EXPECT_EQ(dest->messages.back(), "DEBUG bar.c:1 bar"); // reset successful

    dest->messages.clear();

    auto start2 = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() < start2 + 6s) {
        logger.log(LogLevel::kDebug, std::chrono::system_clock::now(), "foo.c", 1, "foo");
        std::this_thread::sleep_for(800ms); // <2 per second
    }

    EXPECT_NEAR(dest->messages.size(), 10, 2); // no rate limiting
}

TEST(RateLimitedLogger, limitsLogMessagesByFileAndLine){
    auto dest = std::make_shared<TestLogger>();
    RateLimitedLogger logger{dest, 2, 2s, 1s};

    auto start = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() < start + 5s) {
        logger.log(LogLevel::kInfo, std::chrono::system_clock::now(), "foo.c", 1, "foo");
        logger.log(LogLevel::kInfo, std::chrono::system_clock::now(), "bar.c", 1, "foo");
        std::this_thread::sleep_for(100ms); // ~10 per second
    }

    EXPECT_GT(dest->messages.size(), 0);
    EXPECT_LT(dest->messages.size(), 5 * 10 * 2);
    // First four of each type would be allowed + 1 immediate reminder + 1 per
    // second afterwards. Others would be silenced. If this assert fails, it
    // probably means that burst messages aren't working.
    EXPECT_NEAR(dest->messages.size(), 11 * 2, 3 * 2);

    logger.log(LogLevel::kDebug, std::chrono::system_clock::now(), "bar.c", 1, "bar");
    logger.log(LogLevel::kWarning, std::chrono::system_clock::now(), "bar.c", 1, "bar");
    logger.log(LogLevel::kError, std::chrono::system_clock::now(), "bar.c", 1, "bar");
    logger.log(LogLevel::kInfo, std::chrono::system_clock::now(), "bar.c", 1, "bar");

    std::this_thread::sleep_for(3s); // enough time to reset

    logger.log(LogLevel::kInfo, std::chrono::system_clock::now(), "bar.c", 1, "bar");

    ASSERT_GT(dest->messages.size(), 0);
    EXPECT_EQ(dest->messages.back(), "INFO bar.c:1 bar"); // reset successful

    auto start2 = std::chrono::steady_clock::now();

    // overload messages from foo.c:1
    for (auto i = 0; i < 10; ++i) {
        logger.log(LogLevel::kInfo, std::chrono::system_clock::now(), "foo.c", 1, "foo");
    }

    // calm pace for bar.c:1
    while (std::chrono::steady_clock::now() < start2 + 6s) {
        logger.log(LogLevel::kInfo, std::chrono::system_clock::now(), "bar.c", 2, "bar");
        EXPECT_EQ(dest->messages.back(), "INFO bar.c:2 bar"); // reset successful
        std::this_thread::sleep_for(800ms); // <2 per second
    }
}

TEST(FilterLogger, basicUsage) {
    auto dest = std::make_shared<TestLogger>();
    FilterLogger filter{dest, [](LogLevel level, auto...){ return level == LogLevel::kDebug; }};

    filter.log(LogLevel::kDebug, std::chrono::system_clock::now(), "bar.c", 2, "bar");
    EXPECT_EQ(dest->messages.size(), 1);
    EXPECT_EQ(dest->messages.back(), "DEBUG bar.c:2 bar");
    filter.log(LogLevel::kError, std::chrono::system_clock::now(), "bar.c", 2, "bar");
    EXPECT_EQ(dest->messages.size(), 1);
}
