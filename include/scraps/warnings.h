#pragma once

#if !SCRAPS_DISABLE_IGNORE_WARNINGS && defined(__clang__)
    #define SCRAPS_IGNORE_WARNINGS_PUSH                           \
        _Pragma("clang diagnostic push")                          \
        _Pragma("clang diagnostic ignored \"-Wsign-conversion\"") \
        _Pragma("clang diagnostic ignored \"-Wconversion\"")

    #define SCRAPS_IGNORE_WARNINGS_POP                            \
        _Pragma("clang diagnostic pop")

#else
    #define SCRAPS_IGNORE_WARNINGS_PUSH
    #define SCRAPS_IGNORE_WARNINGS_POP
#endif
