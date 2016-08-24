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

#include "scraps/config.h"

#include "scraps/Notification.h"

#include <mutex>
#include <type_traits>

namespace scraps {

class AbstractObserver;

using ObserverList = std::vector<std::shared_ptr<AbstractObserver>>;

class AbstractObserver {
public:
    AbstractObserver() = default;
    AbstractObserver(const AbstractObserver&) = delete;
    AbstractObserver& operator=(const AbstractObserver&) = delete;
    AbstractObserver(AbstractObserver&&) = delete;
    AbstractObserver& operator=(AbstractObserver&&) = delete;
    virtual ~AbstractObserver() = default;

    /**
     * Trigger this observer with the given notification.
     */
    virtual void notify(const std::shared_ptr<AbstractNotification>& notification) const = 0;

    /**
     * Return a hash code for the notification that this observer is linked with.
     */
    virtual size_t typeHashCode() const = 0;
};

template <typename NotificationType>
class ObserverBase : public AbstractObserver {
public:
    virtual ~ObserverBase() = default;

    virtual size_t typeHashCode() const override {
        return typeid(NotificationType).hash_code();
    }
};

template <typename NotificationType, typename T = void>
class Observer;

/**
 * Observer class with a callback that accepts no arguments.
 */
template <typename NotificationType>
class ApatheticObserver : public ObserverBase<NotificationType> {
public:
    using Function = std::function<void()>;

    ApatheticObserver(Function&& func) : _function{std::move(func)} {}

    virtual void notify(const std::shared_ptr<AbstractNotification>& notification) const override {
        std::lock_guard<std::mutex> lock{_mutex};
        SCRAPS_ASSERT(std::dynamic_pointer_cast<NotificationType>(notification));
        _function();
    }

private:
    mutable std::mutex _mutex;
    const Function _function;
};

/**
 * Observer class with a callback that takes the notification as the sole argument.
 */
template <typename NotificationType>
class NotificationObserver : public ObserverBase<NotificationType> {
public:
    using Function = std::function<void(const NotificationType&)>;

    NotificationObserver(Function&& func) : _function{std::move(func)} {}

    virtual void notify(const std::shared_ptr<AbstractNotification>& notification) const override {
        std::lock_guard<std::mutex> lock{_mutex};
        auto casted = std::dynamic_pointer_cast<NotificationType>(notification);
        SCRAPS_ASSERT(casted);
        _function(*casted);
    }

private:
    mutable std::mutex _mutex;
    const Function _function;
};

/**
 * Observer class that invokes arbitrary arguments to the given callback.
 *
 * Example:
 *     class MyNotification : public ArgumentNotification<std::string, uint32_t> {
 *     public:
 *         MyNotification(std::string str, int32_t i) : ArgumentNotification{str, i} {}
 *     };
 *
 *     void MyCallback(const std::string& str, int32_t i) { }
 *
 *     auto observer = std::make_shared<Observer<MyNotification, std::function<void(const std::sting&, int32_t>)>>();
 *
 *     observer->notify(std::make_shared<MyNotification>("hello", 42));
 */
template <typename NotificationType, typename Function>
class ArgumentObserver : public ObserverBase<NotificationType> {
public:
    using Tuple = typename NotificationType::Tuple;

    ArgumentObserver(Function&& func) : _function{std::move(func)} {}

    virtual void notify(const std::shared_ptr<AbstractNotification>& notification) const override {
        std::lock_guard<std::mutex> lock{_mutex};
        auto typedNotification = std::dynamic_pointer_cast<NotificationType>(notification);
        SCRAPS_ASSERT(typedNotification);
        _invoke(typedNotification->data);
    }

private:
    template <bool Done, size_t Total, size_t... N>
    struct Invoker {
        static void Invoke(Function func, const Tuple& data) {
            Invoker<Total == 1 + sizeof...(N), Total, N..., sizeof...(N)>::Invoke(func, data);
        }
    };

    template <size_t Total, size_t... N>
    struct Invoker<true, Total, N...> {
        static void Invoke(Function func, const Tuple& data) {
            func(std::get<N>(data)...);
        }
    };

    void _invoke(const Tuple& data) const {
        typedef typename std::decay<Tuple>::type DecayedTuple;
        Invoker<0 == std::tuple_size<DecayedTuple>::value, std::tuple_size<DecayedTuple>::value>::Invoke(_function, data);
    }

    mutable std::mutex _mutex;
    const Function _function;
};

} // namespace scraps
