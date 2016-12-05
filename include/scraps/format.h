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

#include "scraps/config.h"

#include "fmt/format.h"
#include "fmt/ostream.h"

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
