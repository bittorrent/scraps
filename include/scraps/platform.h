#pragma once

#if __APPLE__
    #include "Availability.h"
    #include "TargetConditionals.h"

    #if TARGET_OS_SIMULATOR
        #define SCRAPS_SIMULATOR 1
    #endif

    #if TARGET_OS_IOS
        #define SCRAPS_IOS 1
    #endif

    #if TARGET_OS_WATCH
        #define SCRAPS_WATCHOS 1
        #if SCRAPS_SIMULATOR
            #define SCRAPS_APPLE_SDK "watchsimulator"
        #else
            #define SCRAPS_APPLE_SDK "watchos"
        #endif
    #endif

    #if SCRAPS_IOS || SCRAPS_WATCHOS
        #define SCRAPS_MOBILE 1
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
        #define SCRAPS_OS_X 1 // the name is just "OS X" now
        #define SCRAPS_MAC_OS_X 1
        #define SCRAPS_APPLE_SDK "macosx"
    #endif

    #if SCRAPS_IOS && !SCRAPS_WATCHOS && !SCRAPS_TVOS
        #if SCRAPS_SIMULATOR
            #define SCRAPS_APPLE_SDK "iphonesimulator"
        #else
            #define SCRAPS_APPLE_SDK "iphoneos"
        #endif
    #endif

    #define SCRAPS_MAC_OS_X_AT_LEAST(version) (MAC_OS_X_VERSION_ ## version and MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_ ## version)

    #define SCRAPS_APPLE 1
    #define SCRAPS_UNIX_LIKE 1

    // require ARC support for all libraries
    #if __OBJC__ && !__has_feature(objc_arc)
        #error Please enable ARC support with -fobjc-arc.
    #endif

#elif __ANDROID__
    #include "scraps/android.h"

    #define SCRAPS_MOBILE 1
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

#if SCRAPS_TVOS
constexpr bool kIsTVOS = true;
#else
constexpr bool kIsTVOS = false;
#endif

#if SCRAPS_MAC_OS_X
constexpr bool kIsMacOSX = true;
#else
constexpr bool kIsMacOSX = false;
#endif

#if SCRAPS_MOBILE
constexpr bool kIsMobile = true;
#else
constexpr bool kIsMobile = false;
#endif

#if SCRAPS_WINDOWS
constexpr bool kIsWindows = true;
#else
constexpr bool kIsWindows = false;
#endif

} } // namespace scraps::platform
