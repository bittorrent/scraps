#pragma once

#include "scraps/config.h"

#if SCRAPS_APPLE

#import <Foundation/Foundation.h>

namespace scraps {

/**
* Not thread-safe.
*
* Store user preferences in an OS-specific way. (e.g. UserDefaults for Mac)
*/
class UserPreferencesImpl {
public:
    UserPreferencesImpl();
    UserPreferencesImpl(const std::string& suiteName);

    UserPreferencesImpl(const UserPreferencesImpl&) = delete;
    UserPreferencesImpl& operator=(const UserPreferencesImpl&) = delete;
    UserPreferencesImpl(UserPreferencesImpl&&) = delete;
    UserPreferencesImpl& operator=(UserPreferencesImpl&&) = delete;

    /**
     * Return true if the given key exists.
     */
    bool has(const std::string& key) const;

    /**
     * Unsets the given key.
     */
    void unset(const std::string& key);

    /**
     * Return a string value for the given key. If not found, the default value is returned.
     */
    std::string getString(const std::string& key) const;

    /**
     * Return an int value for the given key. If not found, then a zero value is returned.
     */
    long long getInt(const std::string& key) const;

    /**
     * Return a double value for the given key. If not found, then a zero value is returned.
     */
    long double getDouble(const std::string& key) const;

    /**
     * Return a bool value for the given key. If not found, then false is returned.
     */
    bool getBool(const std::string& key) const;

    /**
     * Set a string value for the given key.
     */
    void setString(const std::string& key, const std::string& value);

    /**
     * Set an int value for the given key.
     */
    void setInt(const std::string& key, long long value);

    /**
     * Set a double value for the given key.
     */
    void setDouble(const std::string& key, long double value);

    /**
     * Set a bool value for the given key.
     */
    void setBool(const std::string& key, bool value);

private:
    NSUserDefaults* _userDefaults = nil;
};

} // namespace scraps

#endif // SCRAPS_APPLE
