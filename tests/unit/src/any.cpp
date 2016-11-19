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

#include <stdts/any.h>

TEST(any, basicOperation) {
    int i = 5;
    stdts::any a{i};
    EXPECT_EQ(i, stdts::any_cast<int>(a));
    EXPECT_THROW(stdts::any_cast<bool>(a), stdts::bad_any_cast);
}
