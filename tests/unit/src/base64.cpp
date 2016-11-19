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

#include <scraps/base64.h>

using namespace scraps;

TEST(base64, basic) {
    std::string json =
        R"({
    "event": "Signed Up",
    "properties": {
        "distinct_id": "13793",
        "token": "e3bc4100330c35722740fb8c6f5abddc",
        "Referred By": "Friend"
    }
})";

    const auto base64EncodedJSON =
        "ewogICAgImV2ZW50IjogIlNpZ25lZCBVcCIsCiAgICAicHJvcGVydGllcyI6IHsKICAgICAgICAiZGlzdGluY3RfaWQiOiAiMTM3OTMiLAogIC"
        "AgICAgICJ0b2tlbiI6ICJlM2JjNDEwMDMzMGMzNTcyMjc0MGZiOGM2ZjVhYmRkYyIsCiAgICAgICAgIlJlZmVycmVkIEJ5IjogIkZyaWVuZCIK"
        "ICAgIH0KfQ==";

    auto encoded = Base64Encode(json);
    EXPECT_EQ(encoded, base64EncodedJSON);

    auto decoded = Base64Decode(encoded);
    EXPECT_EQ(decoded, json);

    // Since base64 encoding has padding characters depending on the size of the source data, test all cases:
    for (auto i = 0; i < 4; ++i) {
        json.push_back('a');
        encoded = Base64Encode(json);
        decoded = Base64Decode(encoded);
        EXPECT_EQ(decoded, json);
    }
}
