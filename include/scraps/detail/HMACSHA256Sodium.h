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

#include <sodium/crypto_auth_hmacsha256.h>

namespace scraps {
namespace detail {

class HMACSHA256Sodium {
public:
    HMACSHA256Sodium(const void* key, size_t length);

    void reset(const void* key, size_t length);

    void update(const void* data, size_t length);

    void finish(void* result);

private:
    crypto_auth_hmacsha256_state _state;
};

inline void HMACSHA256Sodium::HMACSHA256Sodium(const void* key, size_t length) {
    reset(key, length);
}

inline void HMACSHA256Sodium::reset(const void* key, size_t length) {
    auto err = crypto_auth_hmacsha256_init(&_state, reinterpret_cast<const unsigned char*>(key), length);
    SCRAPS_ASSERT(!err);
}

inline void HMACSHA256Sodium::update(const void* data, size_t length) {
    auto err = crypto_auth_hmacsha256_update(&_state, reinterpret_cast<const unsigned char*>(data), length);
    SCRAPS_ASSERT(!err);
}

inline void HMACSHA256Sodium::finish(void* result) {
    auto err = crypto_auth_hmacsha256_final(&_state, reinterpret_cast<unsigned char*>(result));
    SCRAPS_ASSERT(!err);
}

}} // namespace scraps::detail
