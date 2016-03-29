#include "gtest/gtest.h"

#include "scraps/loggers.h"

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
