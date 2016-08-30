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
#include <gtest/gtest.h>

#include "scraps/hash.h"

using namespace scraps;

static_assert("test"_fnv1a64 == 0xf9e6e6ef197c2b25ull, "fnv1a64 should produce reference hash");
static_assert(R"(
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
)"_fnv1a64, "fnv1a64 should not fail for long strings");
