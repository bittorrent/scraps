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

#include "scraps/Byte.h"

#include <gsl.h>
#include <sodium/crypto_hash_sha256.h>

namespace scraps {

template <typename BaseByteType>
struct SHA256ByteTag {};

template <typename BaseByteType>
using SHA256Byte = StrongByte<SHA256ByteTag<BaseByteType>>;

class SHA256 {
public:
    SHA256() { reset(); }

    static const size_t kHashSize = crypto_hash_sha256_BYTES;

    void reset() {
        crypto_hash_sha256_init(&_state);
    }

    void update(const void* data, size_t length) {
        crypto_hash_sha256_update(&_state, reinterpret_cast<const unsigned char*>(data), length);
    }

    void finish(void* hash) {
        crypto_hash_sha256_final(&_state, reinterpret_cast<unsigned char*>(hash));
    }

private:
    crypto_hash_sha256_state _state;
};

template <typename ByteT, std::ptrdiff_t N>
std::array<SHA256Byte<std::remove_const_t<ByteT>>, SHA256::kHashSize>
GetSHA256(gsl::span<ByteT, N> data) {
    std::array<SHA256Byte<std::remove_const_t<ByteT>>, SHA256::kHashSize> ret;

    SHA256 sha256;
    sha256.update(data.data(), data.size());
    sha256.finish(ret.data());

    return ret;
}

} // namespace scraps
