#pragma once

#include "scraps/config.h"

#include "scraps/platform.h"

#if SCRAPS_APPLE
#include "scraps/apple/UserPreferencesImpl.h"
#else
#error Not implemented for this platform.
#endif

namespace scraps {

/**
* Not thread-safe.
*
* Store user preferences in an OS-specific way. (e.g. UserDefaults for Mac)
*/
class UserPreferences : private UserPreferencesImpl {
public:
    /**
    * @param suiteName if given, constructs preferences that are shared by a suite of applications
    *                  otherwise, constructs preferences for the current application only
    */
    UserPreferences(const std::string& suiteName = "") : UserPreferencesImpl{suiteName} {}

    UserPreferences(const UserPreferences&) = delete;
    UserPreferences& operator=(const UserPreferences&) = delete;
    UserPreferences(UserPreferences&&) = delete;
    UserPreferences& operator=(UserPreferences&&) = delete;

    /**
     * Return true if the given key exists.
     */
    bool has(const std::string& key) const;

    /**
     * Return a string value for the given key. If not found, the default value is returned.
     */
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;

    /**
     * Return an int value for the given key. If not found, then a zero value is returned.
     */
    int64_t getInt(const std::string& key, int64_t defaultValue = 0) const;

    /**
     * Return a double value for the given key. If not found, then a zero value is returned.
     */
    double getDouble(const std::string& key, double defaultValue = 0.0) const;

    /**
     * Return a bool value for the given key. If not found, then false is returned.
     */
    bool getBool(const std::string& key, bool defaultValue = false) const;

    /**
     * Set a string value for the given key.
     */
    void set(const std::string& key, const std::string& value);

    /**
     * Set an int value for the given key.
     */
    void set(const std::string& key, int64_t value);

    /**
     * Set an int value for the given key.
     */
    void set(const std::string& key, int32_t value);

    /**
     * Set a double value for the given key.
     */
    void set(const std::string& key, double value);

    /**
     * Set a bool value for the given key.
     */
    void set(const std::string& key, bool value);
};

inline bool UserPreferences::has(const std::string& key) const {
    return UserPreferencesImpl::has(key);
}

inline std::string UserPreferences::getString(const std::string& key, const std::string& defaultValue) const {
    return UserPreferencesImpl::getString(key, defaultValue);
}

inline int64_t UserPreferences::getInt(const std::string& key, int64_t defaultValue) const {
    if (!has(key)) {
        return defaultValue;
    }
    return UserPreferencesImpl::getInt(key);
}

inline double UserPreferences::getDouble(const std::string& key, double defaultValue) const {
    if (!has(key)) {
        return defaultValue;
    }
    return UserPreferencesImpl::getDouble(key);
}

inline bool UserPreferences::getBool(const std::string& key, bool defaultValue) const {
    if (!has(key)) {
        return defaultValue;
    }
    return UserPreferencesImpl::getBool(key);
}

inline void UserPreferences::set(const std::string& key, const std::string& value) {
    return UserPreferencesImpl::setString(key, value);
}

inline void UserPreferences::set(const std::string& key, int64_t value){
    return UserPreferencesImpl::setInt(key, value);
}

inline void UserPreferences::set(const std::string& key, int32_t value){
    return UserPreferencesImpl::setInt(key, value);
}

inline void UserPreferences::set(const std::string& key, double value) {
    return UserPreferencesImpl::setDouble(key, value);
}

inline void UserPreferences::set(const std::string& key, bool value) {
    return UserPreferencesImpl::setBool(key, value);
}

} // namespace scraps
