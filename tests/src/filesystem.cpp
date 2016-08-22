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
    EXPECT_TRUE(IsDirectory(path + "/sub"));
    EXPECT_TRUE(RemoveDirectory(path + "/sub"));
    EXPECT_TRUE(RemoveDirectory(path));
    EXPECT_FALSE(IsDirectory(path));
}
