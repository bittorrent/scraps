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
#include <scraps/HMACSHA256.h>

using namespace scraps;

TEST(HMACSHA256, basic) {
    std::string key = "key";
    HMACSHA256 digest(key.data(), key.size());
    std::string data = "0123456789";
    digest.update(data.data(), data.size());
    std::array<stdts::byte, HMACSHA256::kResultSize> result;
    digest.finish(result.data());

    EXPECT_EQ("1418bde51967febed1356d35d916222a78b616b4d3d090ec34e3531f41325ed3", ToHex(result));
}
