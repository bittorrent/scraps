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

#if SCRAPS_APPLE
    #include "scraps/detail/SHA256Apple.h"
    namespace scraps { using SHA256Impl = detail::SHA256Apple; }
#else
    #include "scraps/detail/SHA256Sodium.h"
    namespace scraps { using SHA256Impl = detail::SHA256Sodium; }
#endif

#include <gsl.h>

namespace scraps {

template <typename BaseByteType>
struct SHA256ByteTag {};

template <typename BaseByteType>
using SHA256Byte = StrongByte<SHA256ByteTag<BaseByteType>>;

class SHA256 {
public:
    static const size_t kHashSize = 32u;

    void reset();

    void update(const void* data, size_t length);

    void finish(void* hash);

private:
    SHA256Impl _impl;
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

inline void SHA256::reset() {
    _impl.reset();
}

inline void SHA256::update(const void* data, size_t length) {
    _impl.update(data, length);
}

inline void SHA256::finish(void* hash) {
    _impl.finish(hash);
}

} // namespace scraps
