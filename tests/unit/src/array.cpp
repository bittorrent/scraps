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
#include "gtest.h"

#include <scraps/array.h>

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
