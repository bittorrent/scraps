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

#include <scraps/log/CustomFormatter.h>

TEST(CustomFormatter, standard) {
    scraps::log::CustomFormatter formatter;
    scraps::log::Message message{scraps::log::Level::kInfo, "foo.cpp", 1, "text"};
    auto actual = formatter.format(message);

    std::string expected = "[";
    scraps::log::CustomFormatter::AppendTime{}(expected, message);
    expected += "] INFO foo.cpp:1 text";

    EXPECT_EQ(expected, actual);
}

TEST(CustomFormatter, noTime) {
    scraps::log::CustomFormatter::Format format{"{level} {file}:{line} {text}"};
    scraps::log::CustomFormatter formatter{format.functions()};
    auto actual = formatter.format({scraps::log::Level::kInfo, "foo.cpp", 1, "text"});

    EXPECT_EQ("INFO foo.cpp:1 text", actual);
}

TEST(CustomFormatter, json) {
    scraps::log::CustomFormatter::Format format{R"(\{"level"="{level}", "file"="{file}", "line"={line}, "text"="{text}"})"};
    scraps::log::CustomFormatter formatter{format.functions()};
    auto actual = formatter.format({scraps::log::Level::kInfo, "foo.cpp", 1, "text"});

    EXPECT_EQ(R"({"level"="INFO", "file"="foo.cpp", "line"=1, "text"="text"})", actual);
}

TEST(CustomFormatter, noLineNumber) {
    scraps::log::CustomFormatter::Format format{"{level} {file} {text}"};
    scraps::log::CustomFormatter formatter{format.functions()};
    auto actual = formatter.format({scraps::log::Level::kInfo, "foo.cpp", 1, "text"});

    EXPECT_EQ("INFO foo.cpp text", actual);
}

TEST(CustomFormatter, customLevel) {
    auto format = scraps::log::CustomFormatter::Format{"{level} {text}"}
        .set("level", scraps::log::CustomFormatter::AppendLevel{"D","I","W","E"})
    ;
    scraps::log::CustomFormatter formatter{format.functions()};

    EXPECT_EQ("D text", formatter.format({scraps::log::Level::kDebug, "foo.cpp", 1, "text"}));
    EXPECT_EQ("I text", formatter.format({scraps::log::Level::kInfo, "foo.cpp", 1, "text"}));
    EXPECT_EQ("W text", formatter.format({scraps::log::Level::kWarning, "foo.cpp", 1, "text"}));
    EXPECT_EQ("E text", formatter.format({scraps::log::Level::kError, "foo.cpp", 1, "text"}));
}

TEST(CustomFormatter, customLevelFromAttributes) {
    auto format = scraps::log::CustomFormatter::Format{"{level:D,I,W,E} {text}"};
    scraps::log::CustomFormatter formatter{format.functions()};

    EXPECT_EQ("D text", formatter.format({scraps::log::Level::kDebug, "foo.cpp", 1, "text"}));
    EXPECT_EQ("I text", formatter.format({scraps::log::Level::kInfo, "foo.cpp", 1, "text"}));
    EXPECT_EQ("W text", formatter.format({scraps::log::Level::kWarning, "foo.cpp", 1, "text"}));
    EXPECT_EQ("E text", formatter.format({scraps::log::Level::kError, "foo.cpp", 1, "text"}));
}

TEST(CustomFormatter, customTime) {
    auto format = scraps::log::CustomFormatter::Format{"{time} {text}"}
        .set("time",  scraps::log::CustomFormatter::AppendTime{"%T", sizeof("00:00:00")})
    ;

    scraps::log::Message message{scraps::log::Level::kInfo, "foo.cpp", 1, "text"};
    scraps::log::CustomFormatter formatter{format.functions()};
    auto actual = formatter.format(message);

    std::string expected;
    scraps::log::CustomFormatter::AppendTime{"%T"}(expected, message);
    expected += " text";

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(sizeof("00:00:00.000 text")-1, actual.size());
}

TEST(CustomFormatter, customTimeFromAttributes) {
    auto format = scraps::log::CustomFormatter::Format{"{time:%T} {text}"};

    scraps::log::Message message{scraps::log::Level::kInfo, "foo.cpp", 1, "text"};
    scraps::log::CustomFormatter formatter{format.functions()};
    auto actual = formatter.format(message);

    std::string expected;
    scraps::log::CustomFormatter::AppendTime{"%T"}(expected, message);
    expected += " text";

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(sizeof("00:00:00.000 text")-1, actual.size());
}

TEST(CustomFormatter, color) {
    auto format = scraps::log::CustomFormatter::Format{"{color}{level}{/color} {text}"};
    scraps::log::CustomFormatter formatter{format.functions()};

    scraps::log::Message debug  {scraps::log::Level::kDebug,   "foo.cpp", 1, "text"};
    scraps::log::Message info   {scraps::log::Level::kInfo,    "foo.cpp", 1, "text"};
    scraps::log::Message warning{scraps::log::Level::kWarning, "foo.cpp", 1, "text"};
    scraps::log::Message error  {scraps::log::Level::kError,   "foo.cpp", 1, "text"};

    EXPECT_EQ("\x1B[36mDEBUG\x1B[39m text", formatter.format(debug));
    EXPECT_EQ("INFO text", formatter.format(info));
    EXPECT_EQ("\x1B[33mWARNING\x1B[39m text", formatter.format(warning));
    EXPECT_EQ("\x1B[31mERROR\x1B[39m text", formatter.format(error));
}
