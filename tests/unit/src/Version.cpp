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
#include "scraps/Version.h"

#include <gtest/gtest.h>

using namespace scraps;

TEST(Version, basicOperation) {
    Version version{1, 2, 3, 4};

    EXPECT_EQ("1.2.3.4", version.toString());

    EXPECT_EQ(0x0102000300000004u, version.toInteger());

    Version version2{1, 2, 3, 5};

    EXPECT_EQ(version, version);
    EXPECT_NE(version2, version);
    EXPECT_GT(version2, version);
    EXPECT_LT(version, version2);

    Version version3{1, 4, 3, 4};
    EXPECT_NE(version3, version);
    EXPECT_GT(version3, version);
    EXPECT_LT(version, version3);

    EXPECT_EQ(Version("1.2.3.4"), version);

    Version version4{0x0102000300000004u};

    EXPECT_EQ(version4.major, 1);
    EXPECT_EQ(version4.minor, 2);
    EXPECT_EQ(version4.revision, 3);
    EXPECT_EQ(version4.build, 4);
}


TEST(Version, stringConstruction) {
    Version version{"1"};
    EXPECT_EQ("1.0", version.toString());

    version = Version{"1.0"};
    EXPECT_EQ("1.0", version.toString());

    version = Version{"1.2"};
    EXPECT_EQ("1.2", version.toString());

    version = Version{"1.2.3"};
    EXPECT_EQ("1.2.3", version.toString());

    version = Version{"1.2.3.4"};
    EXPECT_EQ("1.2.3.4", version.toString());
}
