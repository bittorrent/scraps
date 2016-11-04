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
#include "scraps/NotificationCenter.h"
#include "scraps/Notification.h"
#include "scraps/Observer.h"

#include <cassert>

namespace scraps {

void NotificationCenter::_addObserver(const std::shared_ptr<AbstractObserver>& observer) {
    std::lock_guard<std::mutex> lock(_mutex);
    std::get_deleter<Deleter>(observer)->it = _observers.emplace(observer->typeHashCode(), observer);
}

void NotificationCenter::_removeObserver(Multimap::iterator it) {
    std::lock_guard<std::mutex> lock(_mutex);
    _observers.erase(it);
}

bool NotificationCenter::_hasObserverForNotification(size_t notificationHashCode) const {
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _observers.find(notificationHashCode);
    return it != _observers.end();
}

void NotificationCenter::postNotification(const std::shared_ptr<AbstractNotification>& notification) const {
    assert(notification);

    std::vector<std::weak_ptr<AbstractObserver>> observers;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        auto& notificationRef = *notification;
        const auto range = _observers.equal_range(typeid(notificationRef).hash_code());
        for (auto it = range.first; it != range.second; ++it) {
            observers.push_back(it->second.lock());
        }
    }

    for (auto& weakObserver : observers) {
        if (auto observer = weakObserver.lock()) {
            observer->notify(notification);
        }
    }
}

bool NotificationCenter::empty() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _observers.empty();
}

std::size_t NotificationCenter::size() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _observers.size();
}

} // namespace scraps
