#pragma once

#include "scraps/config.h"

SCRAPS_IGNORE_WARNINGS_PUSH
#include "cppformat/format.h"
SCRAPS_IGNORE_WARNINGS_POP

namespace scraps {

/**
* Returns a string using the given formatting.
* This is a variadic argument wrapper for the formatting implementation.
*
* Uses printf style syntax.
*
* @return a string using the given formatting
*/
template <typename... Args>
std::string Formatf(const char* format, Args&&... args) {
    try {
        return ::fmt::sprintf(format, std::forward<Args>(args)...);
    } catch (...) {
    }
    return format; // this is probably the best we can do if an exception occurs
}

/**
* Uses modern {} syntax.
*/
template <typename... Args>
std::string Format(const char* format, Args&&... args) {
    try {
        return ::fmt::format(format, std::forward<Args>(args)...);
    } catch (...) {
    }
    return format; // this is probably the best we can do if an exception occurs
}

template <typename... Args>
void Printf(Args&&... args) {
    fputs(Formatf(std::forward<Args>(args)...).c_str(), stdout);
}

template <typename... Args>
void Print(Args&&... args) {
    fputs(Format(std::forward<Args>(args)...).c_str(), stdout);
}

} // namespace scraps
