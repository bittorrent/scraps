#pragma once

#include "scraps/config.h"

#include <thread>
#include <unordered_map>
#include <unordered_set>

namespace scraps {

/**
* Entries in this multi-map expire after a given time period.
*
* Thread-safe.
*/
template <typename Key, typename Value>
class ShortTermMultiMap {
public:
    /**
    * Adds a value for the given key. The entry expires after the given duration.
    */
    void add(Key key, Value value, std::chrono::steady_clock::duration expiration = std::chrono::seconds(10)) {
        std::lock_guard<std::mutex> lock(_mutex);
        _entries[key][value] = std::chrono::steady_clock::now() + expiration;
    }

    /**
    * Adds a value for the given keys. The entry expires after the given duration.
    */
    template <typename Iterable>
    void add(Iterable keys, Value value, std::chrono::steady_clock::duration expiration = std::chrono::seconds(10)) {
        std::lock_guard<std::mutex> lock(_mutex);
        auto t = std::chrono::steady_clock::now() + expiration;
        for (auto& key : keys) {
            _entries[key][value] = t;
        }
    }

    /**
    * Gets all entries.
    */
    std::unordered_map<Key, std::unordered_set<Value>> get() const {
        std::lock_guard<std::mutex> lock(_mutex);
        std::unordered_map<Key, std::unordered_set<Value>> ret;
        for (auto& kv : _entries) {
            ret[kv.first] = _get(kv.first);
        }
        return ret;
    }

    /**
    * Gets the values for the given key.
    */
    std::unordered_set<Value> get(Key key) const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _get(key);
    }

private:
    mutable std::mutex _mutex;
    mutable std::unordered_map<Key, std::unordered_map<Value, std::chrono::steady_clock::time_point>> _entries;

    void _removeExpired() const {
        auto now = std::chrono::steady_clock::now();
        for (auto it = _entries.begin(); it != _entries.end();) {
            for (auto it2 = it->second.begin(); it2 != it->second.end();) {
                if (it2->second < now) {
                    it2 = it->second.erase(it2);
                } else {
                    ++it2;
                }
            }
            if (it->second.empty()) {
                it = _entries.erase(it);
            } else {
                ++it;
            }
        }
    }

    std::unordered_set<Value> _get(Key key) const {
        _removeExpired();

        std::unordered_set<std::string> ret;
        if (!_entries.count(key)) {
            return ret;
        }
        
        auto& entries = _entries[key];
        for (auto& vt : entries) {
            ret.insert(vt.first);
        }
        
        return ret;
    }
};

} // namespace scraps
