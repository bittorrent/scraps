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

#include <algorithm>
#include <vector>
#include <initializer_list>

namespace scraps {

template <typename Key>
class FlatSet {
public:
    using Reference            = typename std::vector<Key>::reference;
    using ConstReference       = typename std::vector<Key>::const_reference;
    using Iterator             = typename std::vector<Key>::iterator;
    using ReverseIterator      = typename std::vector<Key>::reverse_iterator;
    using ConstIterator        = typename std::vector<Key>::const_iterator;
    using ConstReverseIterator = typename std::vector<Key>::const_reverse_iterator;
    using ValueType            = typename std::vector<Key>::value_type;
    using KeyType              = typename std::vector<Key>::value_type;
    using SizeType             = typename std::vector<Key>::size_type;
    using AllocatorType        = typename std::vector<Key>::allocator_type;

    // lower case required for some template specializations such as std::insert_iterator
    using reference              = Reference;
    using const_reference        = ConstReference;
    using iterator               = Iterator;
    using reverse_iterator       = ReverseIterator;
    using const_iterator         = ConstIterator;
    using const_reverse_iterator = ConstReverseIterator;
    using value_type             = ValueType;
    using key_type               = ValueType;
    using size_type              = SizeType;
    using allocator_type         = AllocatorType;

    // Member functions

    FlatSet() = default;

    template <typename InputIt>
    explicit FlatSet(InputIt first, InputIt last) {
        insert(first, last);
    }

    FlatSet(std::initializer_list<Key> init) : FlatSet(init.begin(), init.end()) {}

    // Element access

    const Key* data() const { return _set.data(); }

    ConstReference front() const { return _set.front(); }
    Reference front() { return _set.front(); }

    ConstReference back() const { return _set.back(); }
    Reference back() { return _set.back(); }

    // Iterators, the order of elements must not be modified

    Iterator begin() { return _set.begin(); }
    Iterator end() { return _set.end(); }

    ReverseIterator rbegin() { return _set.rbegin(); }
    ReverseIterator rend() { return _set.rend(); }

    ConstIterator begin() const { return _set.begin(); }
    ConstIterator cbegin() const { return _set.cbegin(); }
    ConstIterator end() const { return _set.end(); }
    ConstIterator cend() const { return _set.cend(); }

    ConstReverseIterator rbegin() const { return _set.rbegin(); }
    ConstReverseIterator crbegin() const { return _set.crbegin(); }
    ConstReverseIterator rend() const { return _set.rend(); }
    ConstReverseIterator crend() const { return _set.crend(); }

    // Capacity

    SizeType size() const { return _set.size(); }
    bool empty() const { return _set.empty(); }
    SizeType maxSize() const { return _set.max_size(); }
    void reserve(SizeType size) { _set.reserve(size); }
    SizeType capacity() const { return _set.capacity(); }

    // Modifiers

    void clear() { _set.clear(); }

    /**
    * logarithmic + linear time
    */
    std::pair<Iterator, bool> insert(const ValueType& value) {
        auto it = lowerBound(value);
        if (it != end() && *it == value) {
            return {it, false};
        }
        return {_set.insert(it, ValueType{value}), true};
    }

    std::pair<Iterator, bool> insert(ValueType&& value) {
        auto it = lowerBound(value);
        if (it != end() && *it == value) {
            return {it, false};
        }
        return {_set.insert(it, std::move(value)), true};
    }

    /**
    * constant + linear time if the key is to be inserted immediately before hint
    * otherwise logarithmic + linear time
    */
    Iterator insert(ConstIterator hint, const ValueType& value) {
        if (hint == end()) {
            if (empty() || *std::prev(hint) < value) {
                return _set.insert(end(), value);
            }
        } else if (*hint == value) {
            // talk about a hack. returns a non-const iterator
            return _set.erase(hint, hint);
        } else if (*hint > value && (hint == begin() || *std::prev(hint) < value)) {
            return {_set.insert(hint, value), true};
        }

        return insert(value).first;
    }

    Iterator insert(ConstIterator hint, ValueType&& value) {
        if (hint == end()) {
            if (_set.empty() || *std::prev(hint) < value) {
                return _set.insert(end(), std::move(value));
            }
        } else if (*hint == value) {
            // talk about a hack. returns a non-const iterator
            return _set.erase(hint, hint);
        } else if (*hint > value && (hint == begin() || *std::prev(hint) < value)) {
            return _set.insert(hint, std::move(value));
        }

        return insert(std::move(value)).first;
    }

    /**
     * Range insertions and emplacements are equivalent to insertion one at a time with no hint. For
     * better performance for sorted ranges, use insert with hints or std::inplace_merge.
     */
    template <typename InputIt>
    void insert(InputIt first, InputIt last) {
        for (auto it = first; it != last; ++it) {
            insert(*it);
        }
    }

    void insert(std::initializer_list<ValueType> ilist) {
        insert(ilist.begin(), ilist.end());
    }

    template <typename... Args>
    std::pair<Iterator, bool> emplace(Args&&... args) {
        return insert(ValueType{std::forward<Args>(args)...});
    }

    template <typename... Args>
    Iterator emplaceHint(ConstIterator hint, Args&&... args) {
        return insert(hint, ValueType{std::forward<Args>(args)...});
    }

    /**
     * linear with distance(pos, end)
     */
    Iterator erase(ConstIterator pos) { return _set.erase(pos); }
    Iterator erase(Iterator pos) { return _set.erase(pos); };

    /**
     * linear with distance(first, last) + distance(last, end)
     */
    Iterator erase(ConstIterator first, ConstIterator last) { return _set.erase(first, last); };

    /**
     * Logarithmic
     */
    SizeType erase(const KeyType& key) {
        auto it = find(key);
        if (it != end()) {
            _set.erase(it);
            return 1;
        }
        return 0;
    }

    void swap(FlatSet<Key>& other) {
        _set.swap(other._set);
    }

    // Lookup

    SizeType count(const KeyType& key) const {
        return find(key) == end() ? 0 : 1;
    }

    template <typename K>
    SizeType count(const K& x) const {
        return find(x) == end() ? 0 : 1;
    }

    Iterator find(const Key& key) {
        auto it = lowerBound(key);
        if (it != end() && *it == key) {
            return it;
        }
        return end();
    }

    ConstIterator find(const Key& key) const {
        auto it = lowerBound(key);
        if (it != end() && *it == key) {
            return it;
        }
        return end();
    }

    template <typename K>
    Iterator find(const K& x) {
        auto it = lowerBound(x);
        if (it != end() && *it == x) {
            return it;
        }
        return end();
    }

    template <typename K>
    ConstIterator find(const K& x) const {
        auto it = lowerBound(x);
        if (it != end() && *it == x) {
            return it;
        }
        return end();
    }

    std::pair<Iterator, Iterator> equalRange(const Key& key) {
        auto first = find(key);
        return {first, first};
    }

    std::pair<ConstIterator, ConstIterator> equalRange( const Key& key ) const {
        auto first = find(key);
        return {first, first};
    }

    template <typename K>
    std::pair<Iterator,Iterator> equalRange(const K& x) {
        auto first = find(x);
        return {first, first};
    }

    template <typename K>
    std::pair<ConstIterator, ConstIterator> equalRange(const K& x) const {
        auto first = find(x);
        return {first, first};
    }

    Iterator lowerBound(const KeyType& key) {
        return std::lower_bound(begin(), end(), key);
    }

    ConstIterator lowerBound(const KeyType& key) const {
        return std::lower_bound(begin(), end(), key);
    }

    template <typename K>
    Iterator lowerBound(const K& x) {
        return std::lower_bound(begin(), end(), x, [&](auto& e, auto& x) { return e < x; });
    }

    template <typename K>
    ConstIterator lowerBound(const K& x) const {
        return std::lower_bound(begin(), end(), x, [&](auto e, auto& x) { return e < x; });
    }

    Iterator upperBound(const KeyType& key) {
        return std::upper_bound(begin(), end(), key);
    }

    ConstIterator upperBound(const KeyType& key) const {
        return std::upper_bound(begin(), end(), key);
    }

    template <typename K>
    Iterator upperBound(const K& x) {
        return std::upper_bound(begin(), end(), x, [&](auto& e, auto& x) { return e < x; });
    }

    template <typename K>
    ConstIterator upperBound(const K& x) const {
        return std::upper_bound(begin(), end(), x, [&](auto& e, auto& x) { return e < x; });
    }

private:
    std::vector<Key> _set;
};

template <typename Key>
bool operator==(const FlatSet<Key>& lhs, const FlatSet<Key>& rhs) {
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Key>
bool operator!=(const FlatSet<Key>& lhs, const FlatSet<Key>& rhs) {
    return !(lhs == rhs);
}

template <typename Key>
bool operator<(const FlatSet<Key>& lhs, const FlatSet<Key>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Key>
bool operator>(const FlatSet<Key>& lhs, const FlatSet<Key>& rhs) {
    return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
}

template <typename Key>
bool operator<=(const FlatSet<Key>& lhs, const FlatSet<Key>& rhs) {
    return !(lhs > rhs);
}

template <typename Key>
bool operator>=(const FlatSet<Key>& lhs, const FlatSet<Key>& rhs) {
    return !(lhs < rhs);
}

} // namespace scraps

namespace std {
    template <class Key>
    void swap(scraps::FlatSet<Key>& lhs, scraps::FlatSet<Key>& rhs) {
        lhs.swap(rhs);
    }
}
