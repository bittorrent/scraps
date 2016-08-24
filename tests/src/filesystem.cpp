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
#include "scraps/filesystem.h"

#include <gtest/gtest.h>

using namespace scraps;

TEST(filesystem, ParentDirectory) {
    EXPECT_FALSE(ParentDirectory("/"));
    EXPECT_EQ(*ParentDirectory("/one/two"), "/one");
    EXPECT_EQ(*ParentDirectory("/one/two/"), "/one/two");
    EXPECT_EQ(*ParentDirectory("/one"), "/");
    EXPECT_EQ(*ParentDirectory("one/two"), "one");
}

TEST(filesystem, creatingAndRemoving) {
    std::string path = "./filesystemTEST";
    if (IsDirectory(path)) {
        EXPECT_TRUE(RemoveDirectory(path));
    }
    EXPECT_FALSE(IsDirectory(path));
    EXPECT_TRUE(CreateDirectory(path + "/sub", true));
    bool iterated = false;
    EXPECT_TRUE(IsDirectory(path + "/sub"));
    EXPECT_TRUE(IterateDirectory(path, [&](const char* name, bool isFile, bool isDirectory) {
        if (name[0] == '.') { return; }
        EXPECT_FALSE(isFile);
        EXPECT_TRUE(isDirectory);
        EXPECT_EQ(name, "sub"s);
        iterated = true;
    }));
    EXPECT_TRUE(iterated);
    EXPECT_TRUE(RemoveDirectory(path + "/sub"));
    EXPECT_TRUE(RemoveDirectory(path));
    EXPECT_FALSE(IsDirectory(path));
}
