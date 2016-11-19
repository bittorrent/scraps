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
#pragma once

#include <scraps/config.h>

#define SCRAPS_MAC_OS_X_AT_LEAST(version) 0

#if __APPLE__
    #include "Availability.h"
    #include "TargetConditionals.h"

    #if TARGET_OS_SIMULATOR
        #define SCRAPS_SIMULATOR 1
    #endif

    #if TARGET_OS_IOS
        #define SCRAPS_IOS 1
        #if SCRAPS_SIMULATOR
            #define SCRAPS_APPLE_SDK "iphonesimulator"
        #else
            #define SCRAPS_APPLE_SDK "iphoneos"
        #endif
    #endif

    #if TARGET_OS_WATCH
        #define SCRAPS_WATCHOS 1
        #if SCRAPS_SIMULATOR
            #define SCRAPS_APPLE_SDK "watchsimulator"
        #else
            #define SCRAPS_APPLE_SDK "watchos"
        #endif
    #endif

    #if TARGET_OS_TV
        #define SCRAPS_TVOS 1
        #if SCRAPS_SIMULATOR
            #define SCRAPS_APPLE_SDK "appletvsimulator"
        #else
            #define SCRAPS_APPLE_SDK "appletvos"
        #endif
    #endif

    #if !SCRAPS_IOS && !SCRAPS_TVOS && !SCRAPS_WATCHOS
        #define SCRAPS_MACOS 1
        #define SCRAPS_APPLE_SDK "macosx"
        #undef SCRAPS_MAC_OS_X_AT_LEAST
        #define SCRAPS_MAC_OS_X_AT_LEAST(version) (MAC_OS_X_VERSION_ ## version and MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_ ## version)
    #endif

    #define SCRAPS_APPLE 1
    #define SCRAPS_UNIX_LIKE 1

    // require ARC support for all libraries
    #if __OBJC__ && !__has_feature(objc_arc)
        #error Please enable ARC support with -fobjc-arc.
    #endif
#elif __ANDROID__
    #define SCRAPS_ANDROID 1
    #define SCRAPS_UNIX_LIKE 1
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #define SCRAPS_LINUX 1
    #define SCRAPS_UNIX_LIKE 1
#elif defined(_WIN32) || defined(_WIN64)
    #define SCRAPS_WINDOWS 1
#endif

namespace scraps {
namespace platform {

#if SCRAPS_ANDROID
    constexpr bool kIsAndroid = true;
#else
    constexpr bool kIsAndroid = false;
#endif

#if SCRAPS_IOS
    constexpr bool kIsIOS = true;
#else
    constexpr bool kIsIOS = false;
#endif

#if SCRAPS_WATCHOS
    constexpr bool kIsWatchOS = true;
#else
    constexpr bool kIsWatchOS = false;
#endif

#if SCRAPS_TVOS
    constexpr bool kIsTVOS = true;
#else
    constexpr bool kIsTVOS = false;
#endif

#if SCRAPS_MACOS
    constexpr bool kIsMacOS = true;
#else
    constexpr bool kIsMacOS = false;
#endif

#if SCRAPS_WINDOWS
    constexpr bool kIsWindows = true;
#else
    constexpr bool kIsWindows = false;
#endif

} } // namespace scraps::platform
