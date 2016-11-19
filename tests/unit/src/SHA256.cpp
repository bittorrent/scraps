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

#include <scraps/hex.h>
#include <scraps/SHA256.h>

using namespace scraps;

TEST(SHA256, basic) {
    SHA256 sha256;
    std::string data = "0123456789";
    sha256.update(data.data(), data.size());
    std::array<unsigned char, SHA256::kHashSize> result;
    sha256.finish(result.data());

    EXPECT_EQ("84d89877f0d4041efb6bf91a16f0248f2fd573e6af05c19f96bedb9f882f7882", ToHex(result));
}
