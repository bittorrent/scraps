#include "scraps/FutureSynchronizer.h"

#include <gtest/gtest.h>

#include <future>
#include <set>

using namespace scraps;

TEST(FutureSynchronizer, basicOperation) {
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
