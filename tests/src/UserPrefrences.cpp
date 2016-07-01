#include "scraps/UserPreferences.h"

#include <gtest/gtest.h>

#if SCRAPS_APPLE

using namespace scraps;

TEST(UserPreferences, strings) {
    UserPreferences userPreferences;
    const std::string key = "key";
    const std::string original = "original";
    const std::string modified = "modified";

    userPreferences.set(key, original);
    ASSERT_EQ(original, userPreferences.get<std::string>(key));

    userPreferences.set(key, modified);
    ASSERT_EQ(modified, userPreferences.get<std::string>(key));
}

TEST(UserPreferences, ints) {
    UserPreferences userPreferences;
    const std::string key = "key";
    const auto original = 42;
    const auto modified = 6;

    userPreferences.set(key, original);
    ASSERT_EQ(original, userPreferences.get<decltype(original)>(key));

    userPreferences.set(key, modified);
    ASSERT_EQ(modified, userPreferences.get<decltype(original)>(key));
}

TEST(UserPreferences, floats) {
    UserPreferences userPreferences;
    const std::string key = "key";
    const auto original = 42.0;
    const auto modified = 6.0;

    userPreferences.set(key, original);
    ASSERT_EQ(original, userPreferences.get<decltype(original)>(key));

    userPreferences.set(key, modified);
    ASSERT_EQ(modified, userPreferences.get<decltype(original)>(key));
}

TEST(UserPreferences, bools) {
    UserPreferences userPreferences;
    const std::string key = "key";
    const auto original = true;
    const auto modified = false;

    userPreferences.set(key, original);
    ASSERT_EQ(original, userPreferences.get<decltype(original)>(key));

    userPreferences.set(key, modified);
    ASSERT_EQ(modified, userPreferences.get<decltype(original)>(key));
}

#endif
