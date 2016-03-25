#pragma once

#include "scraps/config.h"

#include <type_traits>

namespace scraps {

template <typename T, bool little>
class FixedEndian {
public:
    FixedEndian() {}
    FixedEndian(const T& unfixed) : _fixed(_fix(unfixed)) {}
    operator T() const { return _fix(_fixed); }

    static_assert(std::is_integral<T>::value, "FixedEndian can only be used for integers");

private:
    T _fixed;

    union EndiannessTest {
        int32_t i;
        char c[4];

        EndiannessTest() : i(1) {}
        bool isLittle() const { return c[0] != 0; }
    };

    static T _fix(const T& unfixed) {
        EndiannessTest endianness;
        if (endianness.isLittle() == little) {
            return unfixed;
        }

        T ret;

        char* src = (char*)&unfixed;
        char* dst = (char*)&ret + sizeof(T) - 1;

        for (size_t i = 0; i < sizeof(T); ++i, ++src, --dst) {
            *dst = *src;
        }

        return ret;
    }
};

template <typename T>
using LittleEndian = FixedEndian<T, true>;

template <typename T>
using BigEndian = FixedEndian<T, false>;
} // namespace scraps
