#pragma once

#include "scraps/config.h"

#include "scraps/Byte.h"
#include "scraps/Temp.h"

#include <gsl.h>

#include <cerrno>
#include <vector>
#include <iterator>
#include <random>
#include <chrono>

namespace scraps {

namespace {

constexpr uint64_t FNV1A64(const char* str, size_t length, uint64_t hash = UINT64_C(0xcbf29ce484222325)) {
    return length > 0 ? FNV1A64(str + 1, length - 1, (hash ^ (*str & UINT64_C(0xff))) * UINT64_C(0x100000001b3)) : hash;
}

inline namespace literals {
    constexpr uint64_t operator "" _fnv1a64(const char* str, size_t length) {
        return FNV1A64(str, length);
    }
}

}

constexpr double kPi = 3.1415926535897932385;

/**
* Helper function to generate a random integral type within a range
*/
template <class T, class U = T, class Generator>
typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value,
                        typename std::common_type<T, U>::type>::type
UniformDistribution(Generator& g, T min = 0, U max = std::numeric_limits<U>::max()) {
    return std::uniform_int_distribution<typename std::common_type<T, U>::type>(min, max)(g);
}

/**
* Helper function to generate a random real type within a range
*/
template <class T, class U = T, class Generator>
typename std::enable_if<std::is_floating_point<T>::value && std::is_floating_point<U>::value,
                        typename std::common_type<T, U>::type>::type
UniformDistribution(Generator& g, T min = 0.0, U max = 1.0) {
    return std::uniform_real_distribution<typename std::common_type<T, U>::type>(min, max)(g);
}

/**
* Helper function to generate a random chrono::duration within a range
*/
template <class Rep1, class Period1, class Rep2, class Period2, class Generator>
typename std::common_type<std::chrono::duration<Rep1, Period1>, std::chrono::duration<Rep2, Period2>>::type
UniformDistribution(Generator& g,
                    const std::chrono::duration<Rep1, Period1>& min,
                    const std::chrono::duration<Rep2, Period2>& max) {
    using CommonType =
        typename std::common_type<std::chrono::duration<Rep1, Period1>, std::chrono::duration<Rep2, Period2>>::type;
    return CommonType{UniformDistribution(g, CommonType(min).count(), CommonType(max).count())};
}

/**
* Returns vector with n random non-duplicate iterators to elements in the input range, with
* uniform probability, in O(n) time. n must be smaller than the input range length.
*/
template <class InputIt, class Generator>
std::vector<InputIt> NRandomElements(InputIt inFirst, InputIt inLast, size_t n, Generator& g) {
    auto result = std::vector<InputIt>(n);

    for (auto& outIt : result) {
        outIt = inFirst++;
    }

    for (auto count = n; inFirst != inLast; ++inFirst, ++count) {
        auto index = UniformDistribution(g, 0, count);
        if (index < n) {
            result[index] = inFirst;
        }
    }

    return result;
}

template<class Rep, class Period>
constexpr typename std::enable_if<std::chrono::duration<Rep, Period>::min() < std::chrono::duration<Rep, Period>::zero(), std::chrono::duration<Rep, Period>>::type
abs(std::chrono::duration<Rep, Period> d) {
    return d >= d.zero() ? d : -d;
}

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
* Returns a string escaped according to RFC 4627.
*
* @param the string to be escaped
* @return the string escaped according to RFC 4627
*/
std::string JSONEscape(const char* str);

/**
* Returns a string decoded from base64
*
* @param a pointer to the data to be decoded
* @param the length of the data to be decoded
* @return the decoded data stored in a std::string
*
*/
std::string Base64Decode(const char* data, size_t length);

/**
* Returns a string encoded as base64
*
* @param a pointer to the data to be encoded
* @param the length of the data to be encoded
* @return the encoded data stored in a std::string
*
*/
std::string Base64Encode(const char* data, size_t length);

/**
* Returns the basename of a given path.
*/
std::string Basename(const std::string& path);

/**
* Returns the directory name of a given path.
*/
std::string Dirname(const std::string& path);

/**
* Parse out an address and port from the given host string. If a port is not found, the
* defaultPort is returned.
*/
std::tuple<std::string, uint16_t> ParseAddressAndPort(const std::string& host, uint16_t defaultPort);

/**
* Convert from microseconds to a timeval.
*/
timeval ToTimeval(const std::chrono::microseconds& value);

/**
* Allow user to enter a shadowed password from stdin.
*/
std::string GetPasswordFromStdin();

constexpr signed char DecToHex(int c) {
    if (c >= 0 && c < 16) {
        return "0123456789abcdef"[c];
    }
    return -1;
}

constexpr signed char DecToHex(const Byte& c) {
    return DecToHex(c.value());
}

/**
 * Returns a hex string representation of the input span. The output does not
 * include an "0x" prefix.
 *
 * example: ToHex(std::array<uint8_t, 1>{0xAB}) == "AB";
 */
template <typename T, std::ptrdiff_t... BytesDimension>
std::string ToHex(const gsl::span<T, BytesDimension...> range) {
    std::string ret;

    static_assert(sizeof(T) == 1, "Input span type too large");

    ret.reserve(range.size() * 2 + 2);

    for (auto& b : range) {
        ret += DecToHex(b >> 4);
        ret += DecToHex(b & 0x0F);
    }

    return ret;
}

template <typename T, size_t N>
std::string ToHex(const std::array<T, N>& in) {
    return ToHex(gsl::span<const T, N>{in});
}

template <typename CharT>
constexpr int8_t HexToDec(CharT c) {
    if (c >= '0' && c <= '9') { return c - '0'; }
    if (c >= 'a' && c <= 'f') { return c - 'a' + 10; }
    if (c >= 'A' && c <= 'F') { return c - 'A' + 10; }
    return -1;
}

/**
 * Fills a byte range from a range of hex characters. Returns true if the input
 * range is successfully converted to the output range. A prefix of "0x" or "0X"
 * is optional.
 */
template <typename HexCharT, std::ptrdiff_t HexExtent, typename ByteType, std::ptrdiff_t... BytesDimension>
constexpr bool ToBytes(const gsl::basic_string_span<const HexCharT, HexExtent> hex, const gsl::span<ByteType, BytesDimension...> bytes) {
    auto prefixSize = 0;
    if (hex.size() > 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        prefixSize += 2;
    }
    if (hex.size() - prefixSize != bytes.size() * 2) {
        return false;
    }
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        auto hi = HexToDec(hex[i * 2 + prefixSize]);
        auto lo = HexToDec(hex[i * 2 + 1 + prefixSize]);
        if (hi < 0 || lo < 0) {
            return false;
        }
        bytes[i] = ByteType{static_cast<uint8_t>((hi << 4) | lo)};
    }
    return true;
}

template <typename CharT, typename T, size_t N>
constexpr bool ToBytes(const std::basic_string<CharT>& in, std::array<T, N>& out) {
    return ToBytes(gsl::basic_string_span<const CharT>{in}, gsl::span<T, N>{out});
}

/**
* @return the amount of physical memory that the system has, or 0 on error
*/
size_t PhysicalMemory();

} // namespace scraps
