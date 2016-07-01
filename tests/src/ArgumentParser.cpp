#include "NullOStream.h"

#include "scraps/ArgumentParser.h"

#include <gtest/gtest.h>

using namespace scraps;

TEST(ArgumentParser, basicOperation) {
    ArgumentParser parser{"test"};

    bool flag = false;
    bool b = false;
    uint32_t u32 = 0;
    uint64_t u64 = 0;
    int32_t i32 = 0;
    int64_t i64 = 0;
    float f = 0.0f;
    double d = 0.0;

    enum class Enum {
        One, Two
    };

    Enum e;

    parser.addFlag("flag", "flag argument", &flag);
    parser.addArgument("bool", "bool argument", &b);

    parser.addSection("integrals");
    parser.addArgument("int32", "int32 argument", &i32);
    parser.addArgument("int64", "int32 argument", &i64);
    parser.addArgument("uint32", "uint32 argument", &u32);
    parser.addArgument("uint64", "uint64 argument", &u64);

    parser.addSection("floating points");
    parser.addArgument("float", "float argument", &f);
    parser.addArgument("double", "double argument", &d);

    parser.addSection("enum");
    parser.addEnum("enum", "enum argument", &e, "one", Enum::One, "two", Enum::Two);

    NullOStream nullStream;

    parser.help(nullStream);

    std::vector<const char*> argv = {
        "/path/to/program",
        "--flag",
        "--bool",  "true",
        "--int32", "-42",
        "--int64", "-42",
        "--uint32", "42",
        "--uint64", "42",
        "--float", "42.0",
        "--double", "42.0",
        "--enum", "two",
    };

    ASSERT_TRUE(parser.parse(static_cast<int>(argv.size()), &argv[0]));

    EXPECT_TRUE(flag);
    EXPECT_TRUE(b);
    EXPECT_EQ(-42, i32);
    EXPECT_EQ(-42, i64);
    EXPECT_EQ(42, u32);
    EXPECT_EQ(42, u64);
    EXPECT_EQ(42.0f, f);
    EXPECT_EQ(42.0, d);
    EXPECT_EQ(Enum::Two, e);
}
