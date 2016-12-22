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

#include <future>
#include <list>

namespace scraps {

class Continuator {
public:
    Continuator() = default;
    Continuator(const Continuator&) = delete;
    Continuator(Continuator&&) = default;
    Continuator& operator=(const Continuator&) = delete;
    Continuator& operator=(Continuator&&) = default;

    /**
     * Register a callback to be called when the cooresponding future is ready. The
     * provided future must be valid (have a shared state).
     */
    template <typename Future, typename Callback>
    void then(Future f, Callback c);

    /**
     * Checks for futures which are ready and calls cooresponding callbacks.
     */
    void update();

private:
    template <typename Callback, typename T>
    void _callCallback(Callback c, std::future<T> f)         { c(std::move(f)); }
    template <typename Callback>
    void _callCallback(Callback c, std::future<void>)        { c(); }
    template <typename Callback, typename T>
    void _callCallback(Callback c, std::shared_future<T> f)  { c(std::move(f)); }
    template <typename Callback>
    void _callCallback(Callback c, std::shared_future<void>) { c(); }

    std::list<std::function<bool()>> _callbacks;
};

template <typename Future, typename Callback>
void Continuator::then(Future f, Callback c) {
    auto wrapper = [this, f = std::move(f), c = std::move(c)]() mutable {
        if (f.wait_for(0s) == std::future_status::ready) {
            _callCallback(std::move(c), std::move(f));
            return true;
        }
        return false;
    };

    _callbacks.emplace_back([wrapper = std::make_shared<decltype(wrapper)>(std::move(wrapper))] {
        return (*wrapper)();
    });
}

inline void Continuator::update() {
    _callbacks.remove_if([](const auto& i) { return i(); });
}

} // namespace scraps
