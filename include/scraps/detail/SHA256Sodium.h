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

#include <sodium/crypto_hash_sha256.h>

namespace scraps {
namespace detail {

class SHA256Sodium {
public:
    SHA256Sodium();

    void reset();

    void update(const void* data, size_t length);

    void finish(void* hash);

private:
    crypto_hash_sha256_state _state;
};

inline SHA256Sodium::SHA256Sodium() { reset(); }

inline void SHA256Sodium::reset() {
    crypto_hash_sha256_init(&_state);
}

inline void SHA256Sodium::update(const void* data, size_t length) {
    crypto_hash_sha256_update(&_state, reinterpret_cast<const unsigned char*>(data), length);
}

inline void SHA256Sodium::finish(void* hash) {
    crypto_hash_sha256_final(&_state, reinterpret_cast<unsigned char*>(hash));
}

}} // namespace scraps::detail
