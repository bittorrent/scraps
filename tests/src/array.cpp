#include "scraps/array.h"

#include <gtest/gtest.h>

#include <type_traits>

using namespace scraps;

TEST(array, ToArray) {
    {
        int i[5]                    = {0, 1, 2, 3, 4};
        std::array<int, 5> expected = {{0, 1, 2, 3, 4}};

        auto arr = ToArray(i);

        static_assert(std::is_same<decltype(arr), std::array<int, 5>>::value, "test failed");
        EXPECT_EQ(arr, expected);
    }
    {
        int i[5]                    = {0, 1, 2, 3, 4};
        std::array<int, 5> expected = {{0, 1, 2, 3, 4}};

        auto arr = ToArray(gsl::span<int, 5>{i});

        static_assert(std::is_same<decltype(arr), std::array<int, 5>>::value, "test failed");
        EXPECT_EQ(arr, expected);
    }
}

TEST(array, CArray) {
    using Foo       = std::array<uint8_t, 4>;
    using CArrayFoo = uint8_t[4];

    static_assert(std::is_same<CArray<Foo>, CArrayFoo>::value, "test failed");
    static_assert(std::is_same<CArray<Foo&>, CArrayFoo>::value, "test failed");
    static_assert(std::is_same<CArray<const Foo>, CArrayFoo>::value, "test failed");
    static_assert(std::is_same<CArray<const Foo&>, CArrayFoo>::value, "test failed");
}
