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

#include <CommonCrypto/CommonHMAC.h>

namespace scraps {
namespace apple {

enum class HMACAlgorithmType : unsigned int {
    SHA1   = kCCHmacAlgSHA1,
    MD5    = kCCHmacAlgMD5,
    SHA256 = kCCHmacAlgSHA256,
    SHA384 = kCCHmacAlgSHA384,
    SHA224 = kCCHmacAlgSHA224,
    SHA512 = kCCHmacAlgSHA512,
};

constexpr size_t HMACDigestLength(HMACAlgorithmType type) {
    switch (type) {
        case HMACAlgorithmType::SHA1:   return CC_SHA1_DIGEST_LENGTH;
        case HMACAlgorithmType::MD5:    return CC_MD5_DIGEST_LENGTH;
        case HMACAlgorithmType::SHA256: return CC_SHA256_DIGEST_LENGTH;
        case HMACAlgorithmType::SHA384: return CC_SHA384_DIGEST_LENGTH;
        case HMACAlgorithmType::SHA224: return CC_SHA224_DIGEST_LENGTH;
        case HMACAlgorithmType::SHA512: return CC_SHA512_DIGEST_LENGTH;
    }
}

template <HMACAlgorithmType Type>
class HMAC {
public:
    static constexpr size_t kResultSize = HMACDigestLength(Type);

    HMAC(const void* key, size_t length);

    void reset(const void* key, size_t length);

    void update(const void* data, size_t length);

    void finish(void* result);

private:
    CCHmacContext _state;
};

template <HMACAlgorithmType Type>
inline HMAC<Type>::HMAC(const void* key, size_t length) {
    reset(key, length);
}

template <HMACAlgorithmType Type>
inline void HMAC<Type>::reset(const void* key, size_t length) {
    CCHmacInit(&_state, static_cast<std::underlying_type_t<HMACAlgorithmType>>(Type), key, length);
}

template <HMACAlgorithmType Type>
inline void HMAC<Type>::update(const void* data, size_t length) {
    CCHmacUpdate(&_state, data, length);
}

template <HMACAlgorithmType Type>
inline void HMAC<Type>::finish(void* result) {
    CCHmacFinal(&_state, reinterpret_cast<unsigned char*>(result));
}

}} // namespace scraps::apple

#endif
