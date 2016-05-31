#include "scraps/config.h"

#if SCRAPS_ANDROID

#include <jshackle/android/gtest-helper.h>

JSHACKLE_GTEST_MAIN("scraps-tests", scraps_tests, [](auto){})

#endif
