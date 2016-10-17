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
#include <codeformation/Resource.h>

#include <gtest/gtest.h>

using namespace codeformation::types;

struct Foo {
    int i = 0;
};

class FooResource : public codeformation::Resource {
public:
    static constexpr const char* TypeName() { return "Foo"; }

    virtual Any get() override { return _foo.get(); }

    virtual void create() override {
        _foo.reset(new Foo{});
        _foo->i = static_cast<int>(requireProperty<Number>("i"));
    }
private:
    std::unique_ptr<Foo> _foo;
};

TEST(Stack, building) {
    codeformation::Stack stack;
    stack.defineResourceType<FooResource>();

    auto error = stack.build(R"(
        {
            "Resources": {
                "Foobar": {
                    "Type": "Foo",
                    "Properties": {
                        "i": 5
                    }
                }
            },
            "Outputs": {
                "f": { "Ref": "Foobar" },
                "String": "Test"
            }
        }
    )");

    EXPECT_FALSE(error);
    auto f = stack.output<Foo*>("f");
    EXPECT_TRUE(f);
    EXPECT_EQ((*f)->i, 5);
    EXPECT_EQ(*stack.output<String>("String"), "Test");
}

TEST(Stack, constructionWithoutRequiredProperties) {
    codeformation::Stack stack;
    stack.defineResourceType<FooResource>();

    auto error = stack.build(R"(
        {
            "Resources": {
                "Foobar": {
                    "Type": "Foo"
                }
            }
        }
    )");
    EXPECT_TRUE(error);
}

TEST(Stack, cyclicDependency) {
    codeformation::Stack stack;
    stack.defineResourceType<FooResource>();

    auto error = stack.build(R"(
        {
            "Resources": {
                "Foobar": {
                    "Type": "Foo",
                    "Properties": {
                        "i": 0,
                        "Recursion": { "Ref": "Foobar" }
                    }
                }
            }
        }
    )");
    EXPECT_TRUE(error);
}

TEST(Stack, defineFunction) {
    codeformation::Stack stack;
    stack.defineFunction("FooFunc", [](const Any& arg) -> Any {
        EXPECT_EQ(stdts::any_cast<String>(arg), "Foo");
        return String{"Bar"};
    });
    auto error = stack.build(R"(
        {
            "Outputs": {
                "Foo": {"Fn::FooFunc": "Foo"}
            }
        }
    )");
    EXPECT_EQ(*stack.output<String>("Foo"), "Bar");
}
