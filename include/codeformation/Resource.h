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

#include "codeformation/Error.h"
#include "codeformation/types.h"

#include <stdts/any.h>

#include <string>

namespace codeformation {

namespace {
    using namespace std::literals;
    using namespace codeformation::types;
}

class Resource {
public:
    virtual void create() = 0;
    virtual Any get() = 0;

    void setProperties(Dictionary properties) { _properties = std::move(properties); }

protected:

    template <typename T>
    T requireProperty(const String& name) {
        return require<T>(name, _properties);
    }

    template <typename T>
    T optionalProperty(const String& name) {
        return optional<T>(name, _properties);
    }

    template <typename T>
    T require(const Any& thing) {
        auto* ret = stdts::any_cast<T>(&thing);
        if (!ret) {
            throw Error("Invalid type.");
        }
        return *ret;
    }

    template <typename T>
    T require(const String& name, const Dictionary& object) {
        auto it = object.find(name);
        if (it == object.end()) {
            throw Error(name + " is required.");
        }
        auto* ret = stdts::any_cast<T>(&it->second);
        if (!ret) {
            throw Error("The value for "s + name + " is the wrong type.");
        }
        return *ret;
    }

    template <typename T>
    stdts::optional<T> optional(const String& name, const Dictionary& object) {
        auto it = object.find(name);
        if (it == object.end()) {
            return {};
        }
        auto* ret = stdts::any_cast<T>(&it->second);
        if (!ret) {
            throw Error("The value for "s + name + " is the wrong type.");
        }
        return *ret;
    }

private:
    Dictionary _properties;
};

} // namespace codeformation
