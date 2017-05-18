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
#include "../gtest.h"

#include "../tests.h"

#include <stdts/cstddef.h>

TEST(stdts_byte, basics) {
    STATIC_ASSERT_EQ(stdts::byte{0b1} << 1, stdts::byte{0b10});
    STATIC_ASSERT_EQ(stdts::byte{0b1} >> 1, stdts::byte{0b0});
    STATIC_ASSERT_EQ(stdts::byte{0b1} ^ stdts::byte{0b11}, stdts::byte{0b10});
    STATIC_ASSERT_EQ(stdts::byte{0b1} & stdts::byte{0b11}, stdts::byte{0b1});
    STATIC_ASSERT_EQ(stdts::byte{0b1} | stdts::byte{0b10}, stdts::byte{0b11});
    STATIC_ASSERT_EQ(~stdts::byte{0b1}, stdts::byte{0b11111110});

    {
        stdts::byte b{0b111};
        b >>= 1;
        EXPECT_EQ(b, stdts::byte{0b11});
    }
    {
        stdts::byte b{0b1};
        b <<= 1;
        EXPECT_EQ(b, stdts::byte{0b10});
    }
    {
        stdts::byte b{0b111};
        b ^= stdts::byte{0b010};
        EXPECT_EQ(b, stdts::byte{0b101});
    }
    {
        stdts::byte b{0b1};
        b &= stdts::byte{0b10};
        EXPECT_EQ(b, stdts::byte{0b00});
    }
    {
        stdts::byte b{0b01};
        b |= stdts::byte{0b10};
        EXPECT_EQ(b, stdts::byte{0b11});
    }
}
