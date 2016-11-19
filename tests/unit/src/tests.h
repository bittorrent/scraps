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

/**
 * These macros allow us to use static asserts while still tracking code coverage.
 */

#define STATIC_ASSERT_EQ(a, b) \
    static_assert(a == b, ""); \
    ASSERT_EQ(a, b)

#define STATIC_ASSERT_NE(a, b) \
    static_assert(a != b, ""); \
    ASSERT_NE(a, b)
