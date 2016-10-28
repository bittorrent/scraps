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

#if SCRAPS_APPLE
    #include "scraps/detail/HMACApple.h"
    namespace scraps { using HMACSHA256Impl = detail::HMACApple<kCCHmacAlgSHA256>; }
#else
    #include "scraps/detail/SHA256Sodium.h"
    namespace scraps { using HMACSHA256Impl = detail::HMACSHA256Sodium; }
#endif

#include <gsl.h>

namespace scraps {

template <typename BaseByteType>
struct HMACSHA256ByteTag {};

template <typename BaseByteType>
using HMACSHA256Byte = StrongByte<HMACSHA256ByteTag<BaseByteType>>;

class HMACSHA256 {
public:
    static const size_t kResultSize = 32u;

    HMACSHA256(const void* key, size_t length);

    void reset(const void* key, size_t length);

    void update(const void* data, size_t length);

    void finish(void* result);

private:
    HMACSHA256Impl _impl;
};

template <typename KeyByteType, std::ptrdiff_t KeySize, typename ByteT, std::ptrdiff_t DataSize>
std::array<HMACSHA256Byte<std::remove_const_t<ByteT>>, HMACSHA256::kResultSize>
GetHMACSHA256(gsl::span<KeyByteType, KeySize> key, gsl::span<ByteT, DataSize> data) {
    std::array<HMACSHA256Byte<std::remove_const_t<ByteT>>, HMACSHA256::kResultSize> ret;

    HMACSHA256 hmac(key.data(), key.size());
    hmac.update(data.data(), data.size());
    hmac.finish(ret.data());

    return ret;
}

inline HMACSHA256::HMACSHA256(const void* key, size_t length) : _impl{key, length} { }

inline void HMACSHA256::reset(const void* key, size_t length) {
    _impl.reset(key, length);
}

inline void HMACSHA256::update(const void* data, size_t length) {
    _impl.update(data, length);
}

inline void HMACSHA256::finish(void* result) {
    _impl.finish(result);
}

} // namespace scraps
