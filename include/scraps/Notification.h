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

#include <tuple>

namespace scraps {

class AbstractNotification {
protected:
    virtual ~AbstractNotification() = default;
};

namespace detail {
/**
 * Helper class that is used to help tell when a type is an
 * ArgumentNotification type.
 */
class ArgumentNotificationBase : public AbstractNotification {};

} // namespace detail

/**
 * Notification that takes any given numer of parameters as that are passed
 * to an observer.
 */
template <typename... Args>
class ArgumentNotification : public detail::ArgumentNotificationBase {
public:
    using Tuple = std::tuple<Args...>;

    ArgumentNotification(Args... args) : data{std::forward<Args>(args)...} {}

    ArgumentNotification(const Tuple& data) : data{data} {}
    ArgumentNotification(Tuple&& data) : data{std::move(data)} {}

    ArgumentNotification& operator=(const Tuple& data) {
        this->data = data;
        return *this;
    }

    ArgumentNotification& operator=(Tuple&& data) {
        this->data = std::move(data);
        return *this;
    }

    virtual ~ArgumentNotification() = default;

    Tuple data;
};

} // namespace scraps
