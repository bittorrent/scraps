#pragma once
#include "scraps/config.h"

SCRAPS_IGNORE_WARNINGS_PUSH
#include <boost/functional/hash.hpp>
SCRAPS_IGNORE_WARNINGS_POP

#include <ostream>

namespace scraps {

#pragma pack(push, 1)

template <int N>
struct ByteArray {
    bool operator==(const ByteArray& other) const { return !memcmp(bytes, other.bytes, N); }

    bool operator!=(const ByteArray& other) const { return !(*this == other); }

    unsigned char& operator[](int index) { return bytes[index]; }

    unsigned char operator[](int index) const { return bytes[index]; }

    template <typename T>
    T bits(size_t start, size_t count) const {
        T ret = 0;
        for (size_t i = 0; i < count; ++i) {
            auto byte = bytes[(start + i) / 8];
            auto bit  = (start + i) % 8;
            ret <<= 1;
            ret |= ((byte >> (7 - bit)) & 1);
        }
        return ret;
    }

    static ByteArray FromString(const char* str) {
        ByteArray ret;
        for (size_t i = 0; i < N && str[i * 2] && str[i * 2 + 1]; ++i) {
            int byte;
            sscanf(str + i * 2, "%2x", &byte);
            ret[i] = byte;
        }
        return ret;
    }

    unsigned char bytes[N];
};

#pragma pack(pop)

template <int N>
std::ostream& operator<<(std::ostream& os, const ByteArray<N>& array) {
    os.setf(std::ios::right, std::ios::adjustfield);
    os.fill('0');
    for (size_t i = 0; i < N; ++i) {
        os.width(2);
        os << std::hex << (unsigned int)array[i];
    }
    return os;
}

} // namespace scraps

namespace std {
template <int N>
struct hash<scraps::ByteArray<N>> {
    size_t operator()(const scraps::ByteArray<N>& array) const {
        auto data = (unsigned char*)&array;
        return boost::hash_range(data, data + sizeof(array));
    }
};
} // namespace std
