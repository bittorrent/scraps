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
