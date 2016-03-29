#if SCRAPS_MAC_OS_X

#include "gtest/gtest.h"

#include "scraps/UserPreferences.h"

using namespace scraps;

TEST(UserPreferences, strings) {
    UserPreferences userPreferences;
    const std::string key = "key";
    const std::string original = "original";
    const std::string modified = "modified";

    userPreferences.set(key, original);
    ASSERT_EQ(original, userPreferences.getString(key));

    userPreferences.set(key, modified);
    ASSERT_EQ(modified, userPreferences.getString(key));
}

TEST(UserPreferences, ints) {
    UserPreferences userPreferences;
    const std::string key = "key";
    const auto original = 42;
    const auto modified = 6;

    userPreferences.set(key, original);
    ASSERT_EQ(original, userPreferences.getInt(key));

    userPreferences.set(key, modified);
    ASSERT_EQ(modified, userPreferences.getInt(key));
}

TEST(UserPreferences, floats) {
    UserPreferences userPreferences;
    const std::string key = "key";
    const auto original = 42.0;
    const auto modified = 6.0;

    userPreferences.set(key, original);
    ASSERT_EQ(original, userPreferences.getDouble(key));

    userPreferences.set(key, modified);
    ASSERT_EQ(modified, userPreferences.getDouble(key));
}

TEST(UserPreferences, bools) {
    UserPreferences userPreferences;
    const std::string key = "key";
    const auto original = true;
    const auto modified = false;

    userPreferences.set(key, original);
    ASSERT_EQ(original, userPreferences.getBool(key));

    userPreferences.set(key, modified);
    ASSERT_EQ(modified, userPreferences.getBool(key));
}

#endif
