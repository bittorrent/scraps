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

#include <scraps/NotificationCenter.h>

#include <random>
#include <thread>

using namespace scraps;

class TestNotification : public AbstractNotification {};

class TestStringNotification : public ArgumentNotification<std::string> {
public:
    TestStringNotification(const std::string& value) : ArgumentNotification{value} {}
};

class TestMultiArgumentNotification : public ArgumentNotification<bool, int, std::string> {
public:
    TestMultiArgumentNotification(bool b, int i, const std::string& str) : ArgumentNotification{b, i, str} {}
};


TEST(NotificationCenter, basicOperaton) {
    NotificationCenter notificationCenter;
    bool handled = false;
    auto observer = notificationCenter.addObserver<TestNotification>([&]{handled = true;});
    ASSERT_FALSE(notificationCenter.empty());
    ASSERT_EQ(1, notificationCenter.size());

    notificationCenter.postNotification<TestNotification>();
    ASSERT_TRUE(handled);

    observer = nullptr;
    ASSERT_TRUE(notificationCenter.empty());
    ASSERT_EQ(0, notificationCenter.size());
}

TEST(NotificationCenter, multipleNotifications) {
    NotificationCenter notificationCenter;
    bool handled1 = false;
    bool handled2 = false;
    auto observer1 = notificationCenter.addObserver<TestNotification>([&]{handled1 = true;});
    auto observer2 = notificationCenter.addObserver<TestNotification>([&]{handled2 = true;});

    ASSERT_EQ(2, notificationCenter.size());

    notificationCenter.postNotification<TestNotification>();
    ASSERT_TRUE(handled1);
    ASSERT_TRUE(handled2);

    observer1 = nullptr;
    observer2 = nullptr;

    ASSERT_EQ(0, notificationCenter.size());
}

TEST(NotificationCenter, addRemoveNotifications) {
    NotificationCenter notificationCenter;
    bool handled1 = false;
    bool handled2 = false;
    bool handled3 = false;
    auto observer1 = notificationCenter.addObserver<TestNotification>([&]{handled1 = true;});
    auto observer2 = notificationCenter.addObserver<TestNotification>([&]{handled2 = true;});

    ASSERT_EQ(2, notificationCenter.size());

    notificationCenter.postNotification<TestNotification>();
    ASSERT_TRUE(handled1);
    ASSERT_TRUE(handled2);

    observer1 = nullptr;
    observer2 = nullptr;

    handled1 = handled2 = false;

    auto observer3 = notificationCenter.addObserver<TestNotification>([&]{handled3 = true;});
    ASSERT_EQ(1, notificationCenter.size());

    notificationCenter.postNotification<TestNotification>();
    ASSERT_FALSE(handled1);
    ASSERT_FALSE(handled2);
    ASSERT_TRUE(handled3);

    observer3 = nullptr;
}

TEST(NotificationCenter, typedNotifications) {
    NotificationCenter notificationCenter;
    bool handled1 = false;
    std::string handled2;
    auto observer1 = notificationCenter.addObserver<TestNotification>([&](){handled1 = true;});
    auto observer2 = notificationCenter.addObserver<TestStringNotification>([&](const std::string& value){handled2 = value;});

    ASSERT_EQ(2, notificationCenter.size());

    notificationCenter.postNotification<TestNotification>();
    ASSERT_TRUE(handled1);
    ASSERT_TRUE(handled2.empty());
    handled1 = false;

    notificationCenter.postNotification<TestStringNotification>("hello");
    ASSERT_FALSE(handled1);
    ASSERT_EQ("hello", handled2);
}

TEST(NotificationCenter, tupleNotification) {
    NotificationCenter notificationCenter;
    bool boolHandled = false;
    int intHandled = 0;
    std::string stringHandled;
    auto observer = notificationCenter.addObserver<TestMultiArgumentNotification>([&](bool b, int i, const std::string& s){
        boolHandled = b;
        intHandled = i;
        stringHandled = s;
    });

    ASSERT_EQ(1, notificationCenter.size());

    notificationCenter.postNotification<TestMultiArgumentNotification>(true, 42, "hello");
    ASSERT_TRUE(boolHandled);
    ASSERT_EQ(42, intHandled);
    ASSERT_EQ("hello", stringHandled);
}

TEST(NotificationCenter, observerList) {
    NotificationCenter notificationCenter;
    bool handled1 = false;
    std::string handled2;
    {
        ObserverList observers;
        notificationCenter.addObservers(observers)
            .add<TestNotification>([&](){handled1 = true;})
            .add<TestStringNotification>([&](const std::string& value){handled2 = value;});

        ASSERT_EQ(2, notificationCenter.size());

        notificationCenter.postNotification<TestNotification>();
        ASSERT_TRUE(handled1);
        ASSERT_TRUE(handled2.empty());
        handled1 = false;

        notificationCenter.postNotification<TestStringNotification>("hello");
        ASSERT_FALSE(handled1);
        ASSERT_EQ("hello", handled2);
    }

    ASSERT_TRUE(notificationCenter.empty());
}
