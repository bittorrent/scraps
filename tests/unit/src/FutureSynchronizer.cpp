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
#include "scraps/FutureSynchronizer.h"

#include <gtest/gtest.h>

#include <future>
#include <set>

using namespace scraps;

TEST(FutureSynchronizer, async) {
    FutureSynchronizer fs;

    std::set<int> results;

    auto mainThreadId = std::this_thread::get_id();

    auto insert = [&](auto i){
        results.insert(i);
        EXPECT_NE(std::this_thread::get_id(), mainThreadId);
    };

    for (auto i = 0; i < 10; ++i) {
        fs.push(std::async([r = i]{ std::this_thread::sleep_for(100ms); return r; }), insert);
    }

    std::this_thread::sleep_for(2s); // wait for FutureSynchronizer to finish up.

    EXPECT_EQ(results, (std::set<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
}

TEST(FutureSynchronizer, polling) {
    PollingFutureSynchronizer fs;

    std::set<int> results;

    auto mainThreadId = std::this_thread::get_id();

    auto insert = [&](auto i){
        results.insert(i);
        EXPECT_EQ(std::this_thread::get_id(), mainThreadId);
    };

    for (auto i = 0; i < 10; ++i) {
        fs.push(std::async([r = i]{ std::this_thread::sleep_for(100ms); return r; }), insert);
    }

    auto start = std::chrono::steady_clock::now();

    while ((std::chrono::steady_clock::now()-start) < 2s && results.size() != 10) {
        fs.update();
        std::this_thread::sleep_for(100ms);
    }

    EXPECT_EQ(results, (std::set<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
}
