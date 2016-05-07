#pragma once

#include "scraps/config.h"

#include <gsl.h>

namespace scraps {

/**
* Returns a string decoded from base64
*
* @param a pointer to the data to be decoded
* @param the length of the data to be decoded
* @return the decoded data stored in a std::string
*
*/
std::string Base64Decode(const char* data, size_t length);

template <size_t N>
inline std::string Base64Decode(const char (&data)[N]) {
    return Base64Decode(data, N-1);
}

template <typename T, std::ptrdiff_t... BytesDimension>
std::string Base64Decode(const gsl::span<T, BytesDimension...> range) {
    static_assert(sizeof(T) == 1, "Input span type too large");
    return Base64Decode(reinterpret_cast<const char*>(range.data()), range.size());
}

/**
* Returns a string encoded as base64
*
* @param a pointer to the data to be encoded
* @param the length of the data to be encoded
* @return the encoded data stored in a std::string
*
*/
std::string Base64Encode(const char* data, size_t length);

template <size_t N>
inline std::string Base64Encode(const char (&data)[N]) {
    return Base64Encode(data, N-1);
}

template <typename T, std::ptrdiff_t... BytesDimension>
std::string Base64Encode(const gsl::span<T, BytesDimension...> range) {
    static_assert(sizeof(T) == 1, "Input span type too large");
    return Base64Encode(reinterpret_cast<const char*>(range.data()), range.size());
}

} // namespace scraps
