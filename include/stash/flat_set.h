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

#include <algorithm>
#include <initializer_list>
#include <utility>
#include <vector>

namespace stash {

namespace detail {
template <class Compare, typename K = void>
struct IsTransparent {
    using yes = char;
    struct no { char c1; char c; };
    template <class U> static yes test(typename U::is_transparent*);
    template <class U> static no test(...);
    static bool constexpr value = sizeof(test<Compare>(nullptr)) == sizeof(yes);
};

template <typename Compare, typename T1, typename T2>
bool Equivalent(T1&& lhs, T2&& rhs) {
    return !Compare()(lhs, rhs) && !Compare()(rhs, lhs);
}
} // namespace detail

template <
    typename Key,
    class Compare = std::less<Key>
>
class flat_set {
public:
    using reference              = typename std::vector<Key>::const_reference;
    using const_reference        = typename std::vector<Key>::const_reference;
    using iterator               = typename std::vector<Key>::const_iterator;
    using const_iterator         = typename std::vector<Key>::const_iterator;
    using reverse_iterator       = typename std::vector<Key>::const_reverse_iterator;
    using const_reverse_iterator = typename std::vector<Key>::const_reverse_iterator;
    using value_type             = typename std::vector<Key>::value_type;
    using key_type               = typename std::vector<Key>::value_type;
    using size_type              = typename std::vector<Key>::size_type;
    using allocator_type         = typename std::vector<Key>::allocator_type;

    // Constructors

    flat_set() = default;

    template <typename InputIt>
    flat_set(InputIt first, InputIt last);

    flat_set(std::initializer_list<Key> init);

    // Element access

    const_reference front() const;
    const_reference back() const;

    // Iterators

    const_iterator begin() const;
    const_iterator cbegin() const;
    const_iterator end() const;
    const_iterator cend() const;

    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;
    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;

    // Capacity

    size_type size() const;
    size_type max_size() const;
    size_type capacity() const;
    bool empty() const;
    void reserve(size_type size);

    // Modifiers

    void clear();

    /**
    * logarithmic + linear time
    */
    std::pair<const_iterator, bool> insert(const value_type& value);
    std::pair<const_iterator, bool> insert(value_type&& value);

    /**
    * Constant + linear time if the key is to be inserted immediately before hint
    * otherwise logarithmic + linear time
    */
    const_iterator insert(const_iterator hint, const value_type& value);
    const_iterator insert(const_iterator hint, value_type&& value);

    /**
     * Range insertions and emplacements are equivalent to insertion one at a time with no hint
     * (nlog(n)). For better performance for sorted ranges, use insert with hints or
     * std::inplace_merge.
     */
    template <typename InputIt>
    void insert(InputIt first, InputIt last);
    void insert(std::initializer_list<value_type> ilist);

    /**
     * Logarithmic
     */
    template <typename... Args>
    std::pair<const_iterator, bool> emplace(Args&&... args);

    /**
     * Constant + linear time if the key is to be inserted immediately before hint
     * otherwise logarithmic + linear time
     */
    template <typename... Args>
    const_iterator emplace_hint(const_iterator hint, Args&&... args);

    /**
     * Logarithmic + linear with end - pos
     */
    const_iterator erase(const_iterator pos);

    /**
     * Linear with (last - first) + (end - last)
     */
    const_iterator erase(const_iterator first, const_iterator last);

    /**
     * Logarithmic
     */
    size_type erase(const key_type& key);

    /**
     * Linear with resect to container size and number of elements to erase. Elements are moved at
     * most once.
     */
    template <typename Predicate>
    void erase_if(Predicate pred);

    /**
     * Linear with resepct to (end - begin) + (set.end - end). Elements are moved at most once and
     * all elements past end are moved forward to maintain a contiguous series of elements.
     */
    template <typename InputIt, typename Predicate>
    const_iterator erase_if(InputIt begin, InputIt end, Predicate pred);

    /**
     * Constant
     */
    void swap(flat_set<Key, Compare>& other) noexcept(noexcept(_set.swap(other._set)));

    // Lookup

    /**
     * Logarithmic
     */
    size_type count(const key_type& key) const;
    template <typename K>
    auto count(const K& x) const -> std::enable_if_t<detail::IsTransparent<Compare, K>::value, size_type>;

    /**
     * Logarithmic
     */
    const_iterator find(const Key& key) const;
    template <typename K>
    auto find(const K& x) const -> std::enable_if_t<detail::IsTransparent<Compare, K>::value, const_iterator>;

    /**
     * Logarithmic
     */
    std::pair<const_iterator, const_iterator> equal_range(const Key& key) const;
    template <typename K>
    auto equal_range(const K& x) const -> std::enable_if_t<detail::IsTransparent<Compare, K>::value, std::pair<const_iterator, const_iterator>>;

    /**
     * Logarithmic
     */
    const_iterator lower_bound(const key_type& key) const;
    template <typename K>
    auto lower_bound(const K& x) const -> std::enable_if_t<detail::IsTransparent<Compare, K>::value, const_iterator>;

    /**
     * Logarithmic
     */
    const_iterator upper_bound(const key_type& key) const;
    template <typename K>
    auto upper_bound(const K& x) const -> std::enable_if_t<detail::IsTransparent<Compare, K>::value, const_iterator>;

private:
    std::vector<Key> _set;
};

template <typename Key, typename Compare> bool operator==(const flat_set<Key, Compare>& lhs, const flat_set<Key, Compare>& rhs);
template <typename Key, typename Compare> bool operator!=(const flat_set<Key, Compare>& lhs, const flat_set<Key, Compare>& rhs);
template <typename Key, typename Compare> bool operator< (const flat_set<Key, Compare>& lhs, const flat_set<Key, Compare>& rhs);
template <typename Key, typename Compare> bool operator> (const flat_set<Key, Compare>& lhs, const flat_set<Key, Compare>& rhs);
template <typename Key, typename Compare> bool operator<=(const flat_set<Key, Compare>& lhs, const flat_set<Key, Compare>& rhs);
template <typename Key, typename Compare> bool operator>=(const flat_set<Key, Compare>& lhs, const flat_set<Key, Compare>& rhs);

template <typename Key, typename Compare> void swap(stash::flat_set<Key, Compare>& lhs, stash::flat_set<Key, Compare>& rhs) noexcept(noexcept(lhs.swap(rhs)));

// Implementation

// flat_set

template <typename Key, typename Compare>
template <typename InputIt>
flat_set<Key, Compare>::flat_set(InputIt first, InputIt last) {
    insert(first, last);
}

template <typename Key, typename Compare>
flat_set<Key, Compare>::flat_set(std::initializer_list<Key> init) : flat_set(init.begin(), init.end()) {}

template <typename Key, typename Compare> typename flat_set<Key, Compare>::const_reference flat_set<Key, Compare>::front() const { return _set.front(); }
template <typename Key, typename Compare> typename flat_set<Key, Compare>::const_reference flat_set<Key, Compare>::back() const { return _set.back(); }

template <typename Key, typename Compare> typename flat_set<Key, Compare>::const_iterator flat_set<Key, Compare>::begin() const { return _set.begin(); }
template <typename Key, typename Compare> typename flat_set<Key, Compare>::const_iterator flat_set<Key, Compare>::cbegin() const { return _set.cbegin(); }
template <typename Key, typename Compare> typename flat_set<Key, Compare>::const_iterator flat_set<Key, Compare>::end() const { return _set.end(); }
template <typename Key, typename Compare> typename flat_set<Key, Compare>::const_iterator flat_set<Key, Compare>::cend() const { return _set.cend(); }

template <typename Key, typename Compare> typename flat_set<Key, Compare>::const_reverse_iterator flat_set<Key, Compare>::rbegin() const { return _set.rbegin(); }
template <typename Key, typename Compare> typename flat_set<Key, Compare>::const_reverse_iterator flat_set<Key, Compare>::crbegin() const { return _set.crbegin(); }
template <typename Key, typename Compare> typename flat_set<Key, Compare>::const_reverse_iterator flat_set<Key, Compare>::rend() const { return _set.rend(); }
template <typename Key, typename Compare> typename flat_set<Key, Compare>::const_reverse_iterator flat_set<Key, Compare>::crend() const { return _set.crend(); }

template <typename Key, typename Compare> typename flat_set<Key, Compare>::size_type flat_set<Key, Compare>::size() const { return _set.size(); }
template <typename Key, typename Compare> typename flat_set<Key, Compare>::size_type flat_set<Key, Compare>::max_size() const { return _set.max_size(); }
template <typename Key, typename Compare> typename flat_set<Key, Compare>::size_type flat_set<Key, Compare>::capacity() const { return _set.capacity(); }
template <typename Key, typename Compare> bool flat_set<Key, Compare>::empty() const { return _set.empty(); }
template <typename Key, typename Compare> void flat_set<Key, Compare>::reserve(size_type size) { _set.reserve(size); }
template <typename Key, typename Compare> void flat_set<Key, Compare>::clear() { _set.clear(); }

template <typename Key, typename Compare>
std::pair<typename flat_set<Key, Compare>::const_iterator, bool>
flat_set<Key, Compare>::insert(const value_type& value) {
    auto it = lower_bound(value);
    if (it != end() && detail::Equivalent<Compare>(*it, value)) {
        return {it, false};
    }
    return {_set.insert(it, value_type{value}), true};
}

template <typename Key, typename Compare>
std::pair<typename flat_set<Key, Compare>::const_iterator, bool>
flat_set<Key, Compare>::insert(value_type&& value) {
    auto it = lower_bound(value);
    if (it != end() && detail::Equivalent<Compare>(*it, value)) {
        return {it, false};
    }
    return {_set.insert(it, std::move(value)), true};
}

template <typename Key, typename Compare>
typename flat_set<Key, Compare>::const_iterator
flat_set<Key, Compare>::insert(const_iterator hint, const value_type& value) {
    if (hint == end()) {
        if (empty() || Compare()(*std::prev(hint), value)) {
            return _set.insert(end(), value);
        }
    } else if (detail::Equivalent<Compare>(*hint, value)) {
        return hint;
    } else if (Compare()(value, *hint) && (hint == begin() || Compare()(*std::prev(hint), value))) {
        return _set.insert(hint, value);
    }

    return insert(value).first;
}

template <typename Key, typename Compare>
typename flat_set<Key, Compare>::const_iterator
flat_set<Key, Compare>::insert(const_iterator hint, value_type&& value) {
    if (hint == end()) {
        if (_set.empty() || Compare()(*std::prev(hint), value)) {
            return _set.insert(end(), std::move(value));
        }
    } else if (detail::Equivalent<Compare>(*hint, value)) {
        return hint;
    } else if (Compare()(value, *hint) && (hint == begin() || Compare()(*std::prev(hint), value))) {
        return _set.insert(hint, std::move(value));
    }

    return insert(std::move(value)).first;
}

template <typename Key, typename Compare>
template <typename InputIt>
void flat_set<Key, Compare>::insert(InputIt first, InputIt last) {
    for (auto it = first; it != last; ++it) {
        insert(*it);
    }
}

template <typename Key, typename Compare>
void flat_set<Key, Compare>::insert(std::initializer_list<value_type> ilist) {
    insert(ilist.begin(), ilist.end());
}

template <typename Key, typename Compare>
template <typename... Args>
auto flat_set<Key, Compare>::emplace(Args&&... args)
    -> std::pair<typename flat_set<Key, Compare>::const_iterator, bool>
{
    return insert(value_type{std::forward<Args>(args)...});
}

template <typename Key, typename Compare>
template <typename... Args>
auto flat_set<Key, Compare>::emplace_hint(const_iterator hint, Args&&... args)
    -> typename flat_set<Key, Compare>::const_iterator
{
    return insert(hint, value_type{std::forward<Args>(args)...});
}

template <typename Key, typename Compare>
auto flat_set<Key, Compare>::erase(const_iterator pos)
    -> typename flat_set<Key, Compare>::const_iterator
{
    return _set.erase(pos);
}

template <typename Key, typename Compare>
auto flat_set<Key, Compare>::erase(const_iterator first, const_iterator last)
    -> typename flat_set<Key, Compare>::const_iterator
{
    return _set.erase(first, last);
};

template <typename Key, typename Compare>
auto flat_set<Key, Compare>::erase(const key_type& key)
    -> typename flat_set<Key, Compare>::size_type
{
    auto it = find(key);
    if (it != end()) {
        _set.erase(it);
        return 1;
    }
    return 0;
}

template <typename Key, typename Compare>
template <typename Predicate>
void flat_set<Key, Compare>::erase_if(Predicate pred) {
    _set.erase(std::remove_if(_set.begin(), _set.end(), pred), _set.end());
}

template <typename Key, typename Compare>
template <typename InputIt, typename Predicate>
auto flat_set<Key, Compare>::erase_if(InputIt begin, InputIt end, Predicate pred)
    -> typename flat_set<Key, Compare>::const_iterator
{
    // need non-const iterators for std::remove_if
    auto b = _set.erase(begin, begin);
    auto e = _set.erase(end, end);
    return _set.erase(std::remove_if(b, e, pred), e);
}

template <typename Key, typename Compare>
void flat_set<Key, Compare>::swap(flat_set<Key, Compare>& other) noexcept(noexcept(_set.swap(other._set))) {
    _set.swap(other._set);
}

template <typename Key, typename Compare>
auto flat_set<Key, Compare>::count(const key_type& key) const
    -> typename flat_set<Key, Compare>::size_type
{
    return find(key) == end() ? 0 : 1;
}

template <typename Key, typename Compare>
template <typename K>
auto flat_set<Key, Compare>::count(const K& x) const
    -> std::enable_if_t<detail::IsTransparent<Compare, K>::value, typename flat_set::size_type>
{
    return find(x) == end() ? 0 : 1;
}

template <typename Key, typename Compare>
auto flat_set<Key, Compare>::find(const Key& key) const
    -> typename flat_set<Key, Compare>::const_iterator
{
    auto it = lower_bound(key);
    if (it != end() && detail::Equivalent<Compare>(*it, key)) {
        return it;
    }
    return end();
}

template <typename Key, typename Compare>
template <typename K>
auto flat_set<Key, Compare>::find(const K& x) const
    -> std::enable_if_t<detail::IsTransparent<Compare, K>::value, typename flat_set::const_iterator>
{
    auto it = lower_bound(x);
    if (it != end() && detail::Equivalent<Compare>(*it, x)) {
        return it;
    }
    return end();
}

template <typename Key, typename Compare>
auto flat_set<Key, Compare>::equal_range(const Key& key) const
    -> std::pair<typename flat_set<Key, Compare>::const_iterator, typename flat_set<Key, Compare>::const_iterator>
{
    auto first = find(key);
    return {first, first};
}

template <typename Key, typename Compare>
template <typename K>
auto flat_set<Key, Compare>::equal_range(const K& x) const
    -> std::enable_if_t<detail::IsTransparent<Compare, K>::value, std::pair<typename flat_set<Key, Compare>::const_iterator, typename flat_set<Key, Compare>::const_iterator>>
{
    auto first = find(x);
    return {first, first};
}

template <typename Key, typename Compare>
auto flat_set<Key, Compare>::lower_bound(const key_type& key) const
    -> typename flat_set<Key, Compare>::const_iterator
{
    return std::lower_bound(begin(), end(), key, Compare());
}

template <typename Key, typename Compare>
template <typename K>
auto flat_set<Key, Compare>::lower_bound(const K& x) const
    -> std::enable_if_t<detail::IsTransparent<Compare, K>::value, typename flat_set::const_iterator>
{
    return std::lower_bound(begin(), end(), x, [&](auto& e, auto& x) { return Compare()(e, x); });
}

template <typename Key, typename Compare>
auto flat_set<Key, Compare>::upper_bound(const key_type& key) const
    -> typename flat_set<Key, Compare>::const_iterator
{
    return std::upper_bound(begin(), end(), key, Compare());
}

template <typename Key, typename Compare>
template <typename K>
auto flat_set<Key, Compare>::upper_bound(const K& x) const
    -> std::enable_if_t<detail::IsTransparent<Compare, K>::value, typename flat_set::const_iterator>
{
    return std::upper_bound(begin(), end(), x, [&](auto& e, auto& x) { return Compare()(e, x); });
}

// non-member operators

template <typename Key, typename Compare>
bool operator==(const flat_set<Key, Compare>& lhs, const flat_set<Key, Compare>& rhs) {
    return lhs.size() == rhs.size()
        && std::equal(lhs.begin(), lhs.end(), rhs.begin(), [](auto& a, auto& b){
            return detail::Equivalent<Compare>(a, b);
        });
}

template <typename Key, typename Compare>
bool operator!=(const flat_set<Key, Compare>& lhs, const flat_set<Key, Compare>& rhs) {
    return !(lhs == rhs);
}

template <typename Key, typename Compare>
bool operator<(const flat_set<Key, Compare>& lhs, const flat_set<Key, Compare>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), Compare());
}

template <typename Key, typename Compare>
bool operator>(const flat_set<Key, Compare>& lhs, const flat_set<Key, Compare>& rhs) {
    return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end(), Compare());
}

template <typename Key, typename Compare>
bool operator<=(const flat_set<Key, Compare>& lhs, const flat_set<Key, Compare>& rhs) {
    return !(lhs > rhs);
}

template <typename Key, typename Compare>
bool operator>=(const flat_set<Key, Compare>& lhs, const flat_set<Key, Compare>& rhs) {
    return !(lhs < rhs);
}

template <typename Key, typename Compare>
void swap(stash::flat_set<Key, Compare>& lhs, stash::flat_set<Key, Compare>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

} // namespace stash
