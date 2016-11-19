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

#include <codeformation/Error.h>
#include <codeformation/Resource.h>

#include <stdts/optional.h>

#include <json11.hpp>

#include <unordered_map>
#include <unordered_set>
#include <functional>

namespace codeformation {

namespace {
    using namespace codeformation::types;
}

class Stack {
public:
    ~Stack();

    stdts::optional<Error> build(const std::string& json);
    void destroy();

    template <typename T>
    void defineResourceType() {
        _resourceConstructors[T::TypeName()] = []() -> std::unique_ptr<Resource> { return std::make_unique<T>(); };
    }

    template <typename T>
    void defineInput(String name, T&& input) {
        _inputs[name] = input;
    }

    template <typename F>
    void defineFunction(String name, F&& function) {
        _functions[name] = std::forward<F>(function);
    }

    template <typename T>
    stdts::optional<T> output(String name) {
        auto it = _outputs.find(name);
        if (it == _outputs.end()) {
            return {};
        }
        auto* ret = stdts::any_cast<T>(&it->second);
        if (!ret) {
            return {};
        }
        return *ret;
    }

private:
    std::unordered_map<String, std::function<std::unique_ptr<Resource>()>> _resourceConstructors;
    std::unordered_map<String, Any> _inputs;
    std::unordered_map<String, Function> _functions;
    std::unordered_map<String, Any> _outputs;
    std::unordered_map<String, std::unique_ptr<Resource>> _resources;
    std::unordered_set<String> _resourceCreationRequired;
    std::vector<String> _resourceCreationOrder;

    std::unique_ptr<Resource> _createResource(const String& type, Dictionary properties);
    Resource* _resource(const json11::Json& resources, const String& name);
    Dictionary _properties(const json11::Json& resources, const json11::Json& resource);
    Any _evaluate(const json11::Json& resources, const json11::Json& thing);
};

} // namespace codeformation
