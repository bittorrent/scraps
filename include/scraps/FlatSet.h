#pragma once

#include "scraps/config.h"

#include <algorithm>
#include <vector>

namespace scraps {

template <typename T>
class FlatSet {
public:
    FlatSet() = default;

    using ConstIterator        = typename std::vector<T>::const_iterator;
    using ConstReverseIterator = typename std::vector<T>::const_reverse_iterator;
    using Value                = typename std::vector<T>::value_type;

    const T* data() const { return _set.data(); }
    size_t size() const { return _set.size(); }
    bool empty() const { return _set.empty(); }

    ConstIterator begin() const { return _set.begin(); }
    ConstIterator end() const { return _set.end(); }

    ConstReverseIterator rbegin() const { return _set.rbegin(); }
    ConstReverseIterator rend() const { return _set.rend(); }

    void reserve(size_t size) { _set.reserve(size); }
    size_t capacity() const { return _set.capacity(); }

    size_t count(const Value& value) const {
        return std::binary_search(_set.begin(), _set.end(), value) ? 1 : 0;
    }

    /**
    * logarithmic + linear time
    */
    ConstIterator insert(const Value& value) {
        auto it = std::lower_bound(_set.begin(), _set.end(), value);
        if (it != _set.end() && *it == value) {
            return it;
        }
        return _set.insert(it, value);
    }

    /**
    * constant + linear time if the value is to be inserted immediately before hint
    * otherwise logarithmic + linear time
    */
    ConstIterator insert(ConstIterator hint, const Value& value) {
        if (hint == _set.end()) {
            if (_set.empty() || *(hint - 1) < value) {
                return _set.insert(hint, value);
            }
        } else if (*hint == value) {
            return hint;
        } else if (*hint > value && (hint == _set.begin() || *(hint - 1) < value)) {
            return _set.insert(hint, value);
        }

        return insert(value);
    }

    void clear() { _set.clear(); }

private:
    std::vector<T> _set;
};

} // namespace scraps
