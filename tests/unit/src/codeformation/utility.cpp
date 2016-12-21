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

#include <codeformation/utility.h>

TEST(codeformation_utility, ToJson) {
    const codeformation::Dictionary dict{
        {"s", codeformation::String("foo")},
        {"n", codeformation::Number(3.1)},
        {"t", codeformation::Boolean(true)},
        {"f", codeformation::Boolean(false)},
        {"l", codeformation::List{
            codeformation::String("1"),
            codeformation::String("2"),
        }},
    };

    const auto expected = R"({
        "s": "foo",
        "n": 3.1,
        "t": true,
        "f": false,
        "l": ["1", "2"]
    })";

    std::string err;
    const auto expectedJson = json11::Json::parse(expected, err);
    ASSERT_TRUE(err.empty());

    auto json = codeformation::ToJson(dict);
    EXPECT_EQ(json, expectedJson);
}
