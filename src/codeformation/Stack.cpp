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
#include <codeformation/Stack.h>

#include <cassert>

namespace codeformation {

Stack::~Stack() {
    destroy();
}

stdts::optional<Error> Stack::build(const std::string& json) {
    try {
        std::string error;
        auto root = json11::Json::parse(json, error);
        if (!error.empty()) {
            return Error("Error parsing template: "s + error);
        }
        json11::Json resources{json11::Json::object{}};
        if (root.object_items().count("Resources")) {
            resources = root["Resources"];
            for (auto& kv : resources.object_items()) {
                _resource(resources, kv.first);
            }
        }
        if (root.object_items().count("Outputs")) {
            auto outputs = root["Outputs"];
            for (auto& kv : outputs.object_items()) {
                _outputs[kv.first] = _evaluate(resources, kv.second);
            }
        }
    } catch (Error error) {
        return error;
    }
    return {};
}

std::unique_ptr<Resource> Stack::_createResource(const String& type, Dictionary properties){
    std::unique_ptr<Resource> ret;
    auto it = _resourceConstructors.find(type);
    if (it != _resourceConstructors.end()) {
        ret = it->second();
    } else {
        throw Error("Unknown type: "s + type);
    }
    ret->setProperties(std::move(properties));
    ret->create();
    return ret;
}


void Stack::destroy() {
    for (auto it = _resourceCreationOrder.rbegin(); it != _resourceCreationOrder.rend(); ++it) {
        _resources.erase(*it);
    }
}

Resource* Stack::_resource(const json11::Json& resources, const String& name) {
    auto it = _resources.find(name);
    if (it != _resources.end()) {
        return it->second.get();
    }

    if (!resources.object_items().count(name)) {
        throw Error("No resource named "s + name + " was found.");
    }

    try {
        auto resourceObject = resources[name];
        if (!resourceObject.object_items().count("Type")) {
            throw Error("No Type specified.");
        }

        if (_resourceCreationRequired.count(name)) {
            throw Error("Cyclic dependency.");
        }
        _resourceCreationRequired.insert(name);

        auto properties = _properties(resources, resourceObject);
        auto& resource = _resources[name] = _createResource(resourceObject["Type"].string_value(), properties);
        _resourceCreationOrder.emplace_back(name);
        return resource.get();
    } catch (Error error) {
        throw Error(error.message, name);
    }
}

Dictionary Stack::_properties(const json11::Json& resources, const json11::Json& resource) {
    if (!resource.object_items().count("Properties")) {
        return {};
    }

    auto properties = _evaluate(resources, resource["Properties"]);
    auto* map = stdts::any_cast<Dictionary>(&properties);
    if (!map) {
        throw Error("Dictionary expected.");
    }
    return *map;
}

Any Stack::_evaluate(const json11::Json& resources, const json11::Json& thing) {
    switch (thing.type()) {
        case json11::Json::NUL:
            return nullptr;
        case json11::Json::NUMBER:
            return thing.number_value();
        case json11::Json::BOOL:
            return thing.bool_value();
        case json11::Json::STRING:
            return thing.string_value();
        case json11::Json::ARRAY: {
            List ret;
            for (auto& item : thing.array_items()) {
                ret.emplace_back(_evaluate(resources, item));
            }
            return ret;
        }
        case json11::Json::OBJECT: {
            if (thing.object_items().size() == 1) {
                auto key = thing.object_items().begin()->first;
                if (key == "Ref") {
                    auto name = thing[key].string_value();
                    if (_inputs.count(name)) {
                        return _inputs[name];
                    }
                    return _resource(resources, name)->get();
                } else if (key.find("Fn::") == 0) {
                    auto name = key.substr(4);
                    if (!_functions.count(name)) {
                        throw Error("No function named "s + name + " has been defined.");
                    }
                    return _functions[name](_evaluate(resources, thing[key]));
                }
            }
            Dictionary ret;
            for (auto& kv : thing.object_items()) {
                ret[kv.first] = _evaluate(resources, kv.second);
            }
            return ret;
        }
    }
}

} // namespace codeformation
