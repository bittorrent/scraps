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

#include <scraps/Version.h>

using namespace scraps;

TEST(Version, construction) {
        auto version1 = Version{1, 2, 3, 4};
        EXPECT_EQ(version1.major, 1);
        EXPECT_EQ(version1.minor, 2);
        EXPECT_EQ(version1.revision, 3);
        EXPECT_EQ(version1.build, 4);
        EXPECT_EQ("1.2.3.4", version1.toString());
        EXPECT_EQ(0x0102000300000004u, version1.toInt64());

        Version version2 = {1, 2, 3};
        EXPECT_EQ(version2.major, 1);
        EXPECT_EQ(version2.minor, 2);
        EXPECT_EQ(version2.revision, 3);
        EXPECT_EQ(version2.build, 0);
        EXPECT_EQ("1.2.3", version2.toString());
        EXPECT_EQ(0x0102000300000000u, version2.toInt64());
}

TEST(Version, comparison) {
    static_assert(Version{1, 2, 3, 4} >  Version{1, 2, 3}, "");
    static_assert(Version{0, 0, 0, 1} >= Version{}, "");
    static_assert(Version{1, 2, 3}    <  Version{1, 2, 3, 4}, "");
    static_assert(Version{7, 8, 9}    <= Version{7, 9, 8}, "");
    static_assert(Version{}           == Version{0, 0, 0, 0}, "");
    static_assert(Version{12, 23}     != Version{2}, "");
}

TEST(Version, serialization) {
    static_assert(0x0102000300000004u == Version{1, 2, 3, 4}.toInt64(), "");
    static_assert(Version::FromInt64(0x0102000300000004u) == Version{1, 2, 3, 4}, "");

    auto version = Version{1, 2, 3, 4};
    EXPECT_EQ("1.2.3.4", version.toString());
    EXPECT_EQ(0x0102000300000004u, version.toInt64());

    version = Version::FromString("1");
    EXPECT_EQ("1.0", version.toString());
    EXPECT_EQ("1.0.0.0", version.toFullString());

    version = Version::FromString("1.0");
    EXPECT_EQ("1.0", version.toString());

    version = Version::FromString("1.2");
    EXPECT_EQ("1.2", version.toString());

    version = Version::FromString("1.2.3");
    EXPECT_EQ("1.2.3", version.toString());

    version = Version::FromString("1.2.3.4");
    EXPECT_EQ("1.2.3.4", version.toString());
    EXPECT_EQ(0x0102000300000004u, version.toInt64());
}
