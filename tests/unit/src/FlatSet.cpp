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
#include "scraps/FlatSet.h"

#include <gtest/gtest.h>

#include <iostream>

using namespace scraps;

struct Foo {
    Foo() = default;
    explicit Foo(int i) :i{i} {}
    bool operator==(const Foo& rhs) const { return i == rhs.i; }
    bool operator==(int rhs)        const { return this->i == rhs; }
    bool operator<(const Foo& rhs)  const { return i < rhs.i; }
    bool operator<(int rhs)         const { return this->i < rhs; }

    int i = 0;
};

bool operator<(int i, const Foo& f) {
    return i < f.i;
}

TEST(FlatSet, defaultConstruction) {
    FlatSet<int> set;

    EXPECT_TRUE(set.empty());
    EXPECT_EQ(set.begin(), set.end());
}

TEST(FlatSet, rangeConstruction) {
    FlatSet<int> expected{0, 1, 2, 3, 4};

    {
        auto sorted = {0, 1, 2, 3, 4};
        FlatSet<int> set{sorted.begin(), sorted.end()};
        EXPECT_EQ(set, expected);
    }

    {
        auto range = {0, 4, 2, 3, 1};
        FlatSet<int> set{range.begin(), range.end()};
        EXPECT_EQ(set, expected);
    }

    {
        auto dupes = {0, 2, 1, 2, 3, 4, 4, 3};
        FlatSet<int> set{dupes.begin(), dupes.end()};
        EXPECT_EQ(set, expected);
    }

    {
        auto sortedDupes = {0, 0, 1, 2, 2, 3, 3, 4, 4};
        FlatSet<int> set{sortedDupes.begin(), sortedDupes.end()};
        EXPECT_EQ(set, expected);
    }
}

TEST(FlatSet, elementAccess) {
    FlatSet<int> set{0, 1, 2, 3, 4};

    EXPECT_EQ(*set.data() + 0, 0);
    EXPECT_EQ(*set.data() + 1, 1);
    EXPECT_EQ(*set.data() + 2, 2);
    EXPECT_EQ(*set.data() + 3, 3);
    EXPECT_EQ(*set.data() + 4, 4);

    EXPECT_EQ(set.front(), 0);
    EXPECT_EQ(set.back(), 4);
}

TEST(FlatSet, constElementAccess) {
    const FlatSet<int> set{0, 1, 2, 3, 4};

    EXPECT_EQ(*set.data() + 0, 0);
    EXPECT_EQ(*set.data() + 1, 1);
    EXPECT_EQ(*set.data() + 2, 2);
    EXPECT_EQ(*set.data() + 3, 3);
    EXPECT_EQ(*set.data() + 4, 4);

    EXPECT_EQ(set.front(), 0);
    EXPECT_EQ(set.back(), 4);
}

TEST(FlatSet, iterators) {
    FlatSet<int> set{0, 1, 2, 3, 4};

    EXPECT_EQ(std::distance(set.begin(), set.end()), 5);
    EXPECT_EQ(std::distance(set.rbegin(), set.rend()), 5);
}

TEST(FlatSet, constIterators) {
    const FlatSet<int> set{0, 1, 2, 3, 4};

    EXPECT_EQ(std::distance(set.begin(), set.end()), 5);
    EXPECT_EQ(std::distance(set.cbegin(), set.cend()), 5);
    EXPECT_EQ(std::distance(set.rbegin(), set.rend()), 5);
    EXPECT_EQ(std::distance(set.crbegin(), set.crend()), 5);
}

TEST(FlatSet, capacity) {
    FlatSet<int> set{0, 1, 2, 3, 4};

    EXPECT_EQ(set.size(), 5);
    EXPECT_FALSE(set.empty());
    set.reserve(1000);
    EXPECT_GE(set.capacity(), 1000);
}

TEST(FlatSet, insert) {
    {
        FlatSet<int> set{0, 1, 4};
        auto result = set.insert(0);
        EXPECT_EQ(*result.first, 0);
        EXPECT_FALSE(result.second);
        EXPECT_EQ(set, (FlatSet<int>{0, 1, 4}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        auto result = set.insert(3);
        EXPECT_EQ(*result.first, 3);
        EXPECT_TRUE(result.second);
        EXPECT_EQ(set, (FlatSet<int>{0, 1, 3, 4}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        auto result = set.insert(10);
        EXPECT_EQ(*result.first, 10);
        EXPECT_TRUE(result.second);
        EXPECT_EQ(set, (FlatSet<int>{0, 1, 4, 10}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        auto result = set.insert(-10);
        EXPECT_EQ(*result.first, -10);
        EXPECT_TRUE(result.second);
        EXPECT_EQ(set, (FlatSet<int>{-10, 0, 1, 4}));
    }
}

TEST(FlatSet, insertWithHint) {
    {
        FlatSet<int> set{0, 1, 4};
        auto it = set.insert(set.begin(), 0);
        EXPECT_EQ(*it, 0);
        EXPECT_EQ(set, (FlatSet<int>{0, 1, 4}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        auto it = set.insert(set.end(), 3);
        EXPECT_EQ(*it, 3);
        EXPECT_EQ(set, (FlatSet<int>{0, 1, 3, 4}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        auto it = set.insert(set.begin(), 10);
        EXPECT_EQ(*it, 10);
        EXPECT_EQ(set, (FlatSet<int>{0, 1, 4, 10}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        auto it = set.insert(set.end(), 10);
        EXPECT_EQ(*it, 10);
        EXPECT_EQ(set, (FlatSet<int>{0, 1, 4, 10}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        auto it = set.insert(set.begin(), -10);
        EXPECT_EQ(*it, -10);
        EXPECT_EQ(set, (FlatSet<int>{-10, 0, 1, 4}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        auto it = set.insert(set.end(), -10);
        EXPECT_EQ(*it, -10);
        EXPECT_EQ(set, (FlatSet<int>{-10, 0, 1, 4}));
    }
}

TEST(FlatSet, emplace) {
    {
        FlatSet<int> set{0, 1, 4};
        auto result = set.emplace(0);
        EXPECT_EQ(*result.first, 0);
        EXPECT_FALSE(result.second);
        EXPECT_EQ(set, (FlatSet<int>{0, 1, 4}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        auto result = set.emplace(3);
        EXPECT_EQ(*result.first, 3);
        EXPECT_TRUE(result.second);
        EXPECT_EQ(set, (FlatSet<int>{0, 1, 3, 4}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        auto result = set.emplace(10);
        EXPECT_EQ(*result.first, 10);
        EXPECT_TRUE(result.second);
        EXPECT_EQ(set, (FlatSet<int>{0, 1, 4, 10}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        auto result = set.emplace(-10);
        EXPECT_EQ(*result.first, -10);
        EXPECT_TRUE(result.second);
        EXPECT_EQ(set, (FlatSet<int>{-10, 0, 1, 4}));
    }
}

TEST(FlatSet, emplaceWithHint) {
    {
        FlatSet<int> set{0, 1, 4};
        auto it = set.emplaceHint(set.begin(), 0);
        EXPECT_EQ(*it, 0);
        EXPECT_EQ(set, (FlatSet<int>{0, 1, 4}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        auto it = set.emplaceHint(set.end(), 3);
        EXPECT_EQ(*it, 3);
        EXPECT_EQ(set, (FlatSet<int>{0, 1, 3, 4}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        auto it = set.emplaceHint(set.begin(), 10);
        EXPECT_EQ(*it, 10);
        EXPECT_EQ(set, (FlatSet<int>{0, 1, 4, 10}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        auto it = set.emplaceHint(set.end(), 10);
        EXPECT_EQ(*it, 10);
        EXPECT_EQ(set, (FlatSet<int>{0, 1, 4, 10}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        auto it = set.emplaceHint(set.begin(), -10);
        EXPECT_EQ(*it, -10);
        EXPECT_EQ(set, (FlatSet<int>{-10, 0, 1, 4}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        auto it = set.emplaceHint(set.end(), -10);
        EXPECT_EQ(*it, -10);
        EXPECT_EQ(set, (FlatSet<int>{-10, 0, 1, 4}));
    }
}

TEST(FlatSet, erase) {
    {
        FlatSet<int> set{0, 1, 4};
        auto it = set.erase(set.begin());
        EXPECT_EQ(*it, 1);
        EXPECT_EQ(set, (FlatSet<int>{1, 4}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        auto it = set.erase(set.begin(), std::next(set.begin(), 2));
        EXPECT_EQ(*it, 4);
        EXPECT_EQ(set, (FlatSet<int>{4}));
    }

    {
        FlatSet<int> set{0, 1, 4};
        EXPECT_EQ(set.erase(1), 1);
        EXPECT_EQ(set.erase(1), 0);
        EXPECT_EQ(set.erase(5), 0);
        EXPECT_EQ(set, (FlatSet<int>{0, 4}));
    }
}

TEST(FlatSet, swap) {
    FlatSet<int> set1{0, 1, 4};
    FlatSet<int> set2{5, 6, 7};

    set1.swap(set2);

    EXPECT_EQ(set1, (FlatSet<int>{5, 6, 7}));
    EXPECT_EQ(set2, (FlatSet<int>{0, 1, 4}));

    std::swap(set1, set2);

    EXPECT_EQ(set1, (FlatSet<int>{0, 1, 4}));
    EXPECT_EQ(set2, (FlatSet<int>{5, 6, 7}));
}

TEST(FlatSet, count) {

    FlatSet<Foo> set{Foo{1}, Foo{4}, Foo{10}};

    EXPECT_EQ(set.count(Foo{1}), 1);
    EXPECT_EQ(set.count(Foo{5}), 0);

    EXPECT_EQ(set.count(1), 1);
    EXPECT_EQ(set.count(5), 0);
}

TEST(FlatSet, find) {
    {
        FlatSet<int> set{1, 4, 10};
        EXPECT_EQ(set.find(6), set.end());
        EXPECT_EQ(set.find(1), set.begin());
    }

    {
        FlatSet<Foo> set{Foo{1}, Foo{4}, Foo{10}};
        EXPECT_EQ(set.find(6), set.end());
        EXPECT_EQ(set.find(1), set.begin());
    }
}

TEST(FlatSet, constFind) {
    {
        const FlatSet<int> set{1, 4, 10};
        EXPECT_EQ(set.find(6), set.end());
        EXPECT_EQ(set.find(1), set.begin());
    }

    {
        const FlatSet<Foo> set{Foo{1}, Foo{4}, Foo{10}};
        EXPECT_EQ(set.find(6), set.end());
        EXPECT_EQ(set.find(1), set.begin());
    }
}

TEST(FlatSet, equalRange) {
    {
        FlatSet<int> set{1, 4, 10};
        EXPECT_EQ(set.equalRange(6), std::make_pair(set.end(), set.end()));
        EXPECT_EQ(set.equalRange(1), std::make_pair(set.begin(), set.begin()));
    }

    {
        FlatSet<Foo> set{Foo{1}, Foo{4}, Foo{10}};
        EXPECT_EQ(set.equalRange(6), std::make_pair(set.end(), set.end()));
        EXPECT_EQ(set.equalRange(1), std::make_pair(set.begin(), set.begin()));
    }
}

TEST(FlatSet, constEqualRange) {
    {
        const FlatSet<int> set{1, 4, 10};
        EXPECT_EQ(set.equalRange(6), std::make_pair(set.end(), set.end()));
        EXPECT_EQ(set.equalRange(1), std::make_pair(set.begin(), set.begin()));
    }

    {
        const FlatSet<Foo> set{Foo{1}, Foo{4}, Foo{10}};
        EXPECT_EQ(set.equalRange(6), std::make_pair(set.end(), set.end()));
        EXPECT_EQ(set.equalRange(1), std::make_pair(set.begin(), set.begin()));
    }
}

TEST(FlatSet, lowerBound) {
    {
        FlatSet<int> set{1, 4, 10};
        EXPECT_EQ(set.lowerBound(20), set.end());
        EXPECT_EQ(set.lowerBound(1), set.begin());
    }

    {
        FlatSet<Foo> set{Foo{1}, Foo{4}, Foo{10}};
        EXPECT_EQ(set.lowerBound(20), set.end());
        EXPECT_EQ(set.lowerBound(1), set.begin());
    }
}

TEST(FlatSet, constLowerBound) {
    {
        const FlatSet<int> set{1, 4, 10};
        EXPECT_EQ(set.lowerBound(20), set.end());
        EXPECT_EQ(set.lowerBound(1), set.begin());
    }

    {
        const FlatSet<Foo> set{Foo{1}, Foo{4}, Foo{10}};
        EXPECT_EQ(set.lowerBound(20), set.end());
        EXPECT_EQ(set.lowerBound(1), set.begin());
    }
}

TEST(FlatSet, upperBound) {
    {
        FlatSet<int> set{1, 4, 10};
        EXPECT_EQ(set.upperBound(20), set.end());
        EXPECT_EQ(set.upperBound(1), std::next(set.begin()));
    }

    {
        FlatSet<Foo> set{Foo{1}, Foo{4}, Foo{10}};
        EXPECT_EQ(set.upperBound(20), set.end());
        EXPECT_EQ(set.upperBound(1), std::next(set.begin()));
    }
}

TEST(FlatSet, constUpperBound) {
    {
        const FlatSet<int> set{1, 4, 10};
        EXPECT_EQ(set.upperBound(20), set.end());
        EXPECT_EQ(set.upperBound(1), std::next(set.begin()));
    }

    {
        const FlatSet<Foo> set{Foo{1}, Foo{4}, Foo{10}};
        EXPECT_EQ(set.upperBound(20), set.end());
        EXPECT_EQ(set.upperBound(1), std::next(set.begin()));
    }
}

TEST(FlatSet, operators) {
    FlatSet<int> set1{0, 1, 2};
    FlatSet<int> set2{0, 2, 3};
    FlatSet<int> set3{0, 1, 2, 3};

    // 1 vs 1
    EXPECT_TRUE (set1 == set1);
    EXPECT_FALSE(set1 != set1);
    EXPECT_FALSE(set1 <  set1);
    EXPECT_TRUE (set1 <= set1);
    EXPECT_FALSE(set1 >  set1);
    EXPECT_TRUE (set1 >= set1);

    // 1 vs 2
    EXPECT_FALSE(set1 == set2);
    EXPECT_TRUE (set1 != set2);
    EXPECT_TRUE (set1 <  set2);
    EXPECT_TRUE (set1 <= set2);
    EXPECT_FALSE(set1 >  set2);
    EXPECT_FALSE(set1 >= set2);

    // 1 vs 3
    EXPECT_FALSE(set1 == set3);
    EXPECT_TRUE (set1 != set3);
    EXPECT_TRUE (set1 <  set3);
    EXPECT_TRUE (set1 <= set3);
    EXPECT_FALSE(set1 >  set3);
    EXPECT_FALSE(set1 >= set3);

    // 2 vs 1
    EXPECT_FALSE(set2 == set1);
    EXPECT_TRUE (set2 != set1);
    EXPECT_FALSE(set2 <  set1);
    EXPECT_FALSE(set2 <= set1);
    EXPECT_TRUE (set2 >  set1);
    EXPECT_TRUE (set2 >= set1);

    // 2 vs 2
    EXPECT_TRUE (set2 == set2);
    EXPECT_FALSE(set2 != set2);
    EXPECT_FALSE(set2 <  set2);
    EXPECT_TRUE (set2 <= set2);
    EXPECT_FALSE(set2 >  set2);
    EXPECT_TRUE (set2 >= set2);

    // 2 vs 3
    EXPECT_FALSE(set2 == set3);
    EXPECT_TRUE (set2 != set3);
    EXPECT_FALSE(set2 <  set3);
    EXPECT_FALSE(set2 <= set3);
    EXPECT_TRUE (set2 >  set3);
    EXPECT_TRUE (set2 >= set3);

    // 3 vs 1
    EXPECT_FALSE(set3 == set1);
    EXPECT_TRUE (set3 != set1);
    EXPECT_FALSE(set3 <  set1);
    EXPECT_FALSE(set3 <= set1);
    EXPECT_TRUE (set3 >  set1);
    EXPECT_TRUE (set3 >= set1);

    // 3 vs 2
    EXPECT_FALSE(set3 == set2);
    EXPECT_TRUE (set3 != set2);
    EXPECT_TRUE (set3 <  set2);
    EXPECT_TRUE (set3 <= set2);
    EXPECT_FALSE(set3 >  set2);
    EXPECT_FALSE(set3 >= set2);

    // 3 vs 3
    EXPECT_TRUE (set3 == set3);
    EXPECT_FALSE(set3 != set3);
    EXPECT_FALSE(set3 <  set3);
    EXPECT_TRUE (set3 <= set3);
    EXPECT_FALSE(set3 >  set3);
    EXPECT_TRUE (set3 >= set3);
}
