#include "scraps/config.h"

#if SCRAPS_ANDROID

#include <apk-wrapper/gtest-android-helper.h>

APK_WRAPPER_GTEST_MAIN("scraps-tests", scraps_tests, [](auto){})

#endif
