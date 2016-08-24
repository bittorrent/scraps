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
