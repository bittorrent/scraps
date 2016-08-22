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
