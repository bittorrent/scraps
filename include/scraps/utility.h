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

#include "scraps/base64.h"
#include "scraps/Byte.h"
#include "scraps/hex.h"
#include "scraps/Temp.h"
#include "scraps/random.h"
#include "scraps/hash.h"
#include "stdts/optional.h"

#include <gsl.h>

#include <cerrno>
#include <vector>
#include <iterator>
#include <random>
#include <chrono>

namespace scraps {

constexpr double kPi = 3.1415926535897932385;

/**
* Returns a string escaped according to RFC 4627.
*
* @param the string to be escaped
* @return the string escaped according to RFC 4627
*/
std::string JSONEscape(const char* str);

/**
* Returns a string in which all non-alphanumeric characters except dashes, underscores,
* spaces, and periods are replaced with a percent sign followed by their hexadecimal
* value. Spaces are replaced with plus signs.
*
* @return the url-encoded string
*/
std::string URLEncode(const char* str);

inline std::string URLEncode(const std::string& str) { return URLEncode(str.c_str()); }

/**
* Returns a string in which the effects of URLEncode have been reversed.
*
* @return the url-decoded string
*/
std::string URLDecode(const char* str);

inline std::string URLDecode(const std::string& str) { return URLDecode(str.c_str()); }

/**
 * Clamp a value between min and max, inclusive.
 */
template <typename T, typename MinT, typename MaxT>
constexpr auto Clamp(const T& value, const MinT& min, const MaxT& max) {
    return std::max<std::common_type_t<T, MinT, MaxT>>(min, std::min<std::common_type_t<T, MinT, MaxT>>(value, max));
}

/**
* Parse out an address and port from the given host string. If a port is not found, the
* defaultPort is returned.
*/
std::tuple<std::string, uint16_t> ParseAddressAndPort(const std::string& host, uint16_t defaultPort);

/**
* @return the amount of physical memory that the system has, or 0 on error
*/
size_t PhysicalMemory();

/**
* Iterates over a container, allowing for safe modification of the container at any point.
*/
template <typename T, typename F>
void NonatomicIteration(T&& iterable, F&& function) {
    auto copy = iterable;
    for (auto& element : copy) {
        auto it = std::find(iterable.begin(), iterable.end(), element);
        if (it == iterable.end()) {
            continue;
        }
        function(element);
    }
}

template <typename T, std::ptrdiff_t N>
gsl::basic_string_span<T> TrimLeft(gsl::basic_string_span<T, N> str) {
    size_t i = 0;
    while(i < str.size() && isspace(str[i])) { ++i; };
    return {str.data() + i, static_cast<std::ptrdiff_t>(str.size() - i)};
}

template <typename T, std::ptrdiff_t N>
gsl::basic_string_span<T> TrimRight(gsl::basic_string_span<T, N> str) {
    size_t i = str.size();
    while(i > 0 && isspace(str[i - 1])) { --i; };
    return {str.data(), static_cast<std::ptrdiff_t>(i)};
}

template <typename T, std::ptrdiff_t N>
auto Trim(gsl::basic_string_span<T, N> str) {
    return TrimLeft(TrimRight(str));
}

stdts::optional<std::vector<Byte>> BytesFromFile(const std::string& path);

/**
* Sets the given file descriptor to blocking or non-blocking.
*
* @param fd the file descriptor
* @param blocking true if the file descriptor should be set to blocking. false if it should be set to non-blocking
* @return true on success
*/
bool SetBlocking(int fd, bool blocking = true);

/**
* Returns a demangled symbol name. If demangling is not supported, returns original mangled name.
*/
std::string Demangle(const char* name);

/**
* Returns whether two strings are equal disregarding case
*/
inline bool CaseInsensitiveEquals(stdts::string_view l, stdts::string_view r) {
    return l.size() == r.size() && std::equal(l.begin(), l.end(), r.begin(), [](char a, char b){ return tolower(a) == tolower(b); });
}

} // namespace scraps
