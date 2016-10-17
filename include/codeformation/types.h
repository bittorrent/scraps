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
#pragma once

#include <stdts/any.h>

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>

namespace codeformation {
namespace types {

using Any = stdts::any;
using Boolean = bool;
using Number = double;
using String = std::string;
using Dictionary = std::unordered_map<std::string, Any>;
using List = std::vector<Any>;
using Function = std::function<Any(const Any&)>;

} // namespace types

namespace {
    using namespace types;
} // anonymous namespace

} // namespace codeformation
