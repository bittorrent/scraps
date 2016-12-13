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
#include "../gtest.h"
#include "TestLogger.h"

#include <scraps/log/FileLogger.h>

TEST(FileLogger, DefaultLogPath) {
    const auto actual = scraps::log::FileLogger::DefaultLogPath("test");
#if SCRAPS_MACOS
    EXPECT_NE(actual.find("/Library/Logs/test/"), std::string::npos);
#elif SCRAPS_LINUX
    EXPECT_EQ("/var/log/test/", actual);
#elif SCRAPS_WINDOWS
    EXPECT_NE(actual.find("\\test\\"), std::string::npos);
#endif
}
