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

#include <CommonCrypto/CommonDigest.h>

namespace scraps {
namespace apple {

class SHA256 {
public:
    static constexpr size_t kHashSize = CC_SHA256_DIGEST_LENGTH;

    SHA256();

    void reset();

    void update(const void* data, size_t length);

    void finish(void* hash);

private:
    CC_SHA256_CTX _state;
};

inline SHA256::SHA256() { reset(); }

inline void SHA256::reset() {
    CC_SHA256_Init(&_state);
}

inline void SHA256::update(const void* data, size_t length) {
    CC_SHA256_Update(&_state, data, length);
}

inline void SHA256::finish(void* hash) {
    CC_SHA256_Final(reinterpret_cast<unsigned char*>(hash), &_state);
}

}} // namespace scraps::apple

#endif
