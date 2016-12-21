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
#include <codeformation/utility.h>

namespace codeformation {

stdts::optional<json11::Json> ToJson(const Any& any) {
    if (auto x = stdts::any_cast<Number>(&any)) {
        return json11::Json{*x};
    } else if (auto x = stdts::any_cast<Boolean>(&any)) {
        return json11::Json{*x};
    } else if (auto x = stdts::any_cast<String>(&any)) {
        return json11::Json{*x};
    } else if (auto x = stdts::any_cast<Dictionary>(&any)) {
        json11::Json::object ret;
        for (auto& kv : *x) {
            if (auto json = ToJson(kv.second)) {
                ret[kv.first] = std::move(*json);
            } else {
                return {};
            }
        }
        return json11::Json{std::move(ret)};
    } else if (auto x = stdts::any_cast<List>(&any)) {
        json11::Json::array ret;
        for (auto& item : *x) {
            if (auto json = ToJson(item)) {
                ret.emplace_back(std::move(*json));
            } else {
                return {};
            }
        }
        return json11::Json{std::move(ret)};
    }
    return {};
}

} // namespace codeformation
