#pragma once

#include "scraps/config.h"

#include "scraps/platform.h"

#if SCRAPS_APPLE
#include "scraps/apple/UserPreferencesImpl.h"
#elif SCRAPS_ANDROID || SCRAPS_LINUX
// TODO: implement for android, linux
#include "scraps/standins/DummyUserPreferencesImpl.h"
#else
#error Not implemented for this platform.
#endif

namespace scraps {

#if SCRAPS_ANDROID || SCRAPS_LINUX
using UserPreferencesImpl = standins::DummyUserPreferencesImpl;
#endif

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
    UserPreferences(std::string suiteName = "");

    UserPreferences(const UserPreferences&) = delete;
    UserPreferences& operator=(const UserPreferences&) = delete;
    UserPreferences(UserPreferences&&) = delete;
    UserPreferences& operator=(UserPreferences&&) = delete;

    /**
     * Return true if the given key exists.
     */
    bool has(const std::string& key) const;

    /**
     * Unsets the given key.
     */
    void unset(const std::string& key);

    /**
     * Convert the raw type to T. If not found, the default value is returned.
     */
    template <typename T, typename U = typename std::remove_reference<typename std::remove_cv<T>::type>::type>
    U get(const std::string& key, T defaultValue = T{}) const;

    /**
     * Set a bool value for the given key.
     */
    void set(const std::string& key, bool value);

    /**
     * Set an int value for the given key.
     */
    void set(const std::string& key, int value);

    /**
     * Set an int value for the given key.
     */
    void set(const std::string& key, unsigned value);

    /**
     * Set an int value for the given key.
     */
    void set(const std::string& key, long long value);

    /**
     * Set an int value for the given key.
     */
    void set(const std::string& key, unsigned long long value);

    /**
     * Set an int value for the given key.
     */
    void set(const std::string& key, long value);

    /**
     * Set an int value for the given key.
     */
    void set(const std::string& key, unsigned long value);

    /**
     * Set a double value for the given key.
     */
    void set(const std::string& key, float value);

    /**
     * Set a double value for the given key.
     */
    void set(const std::string& key, double value);

    /**
     * Set a double value for the given key.
     */
    void set(const std::string& key, long double value);

    /**
     * Set a string value for the given key.
     */
    void set(const std::string& key, const std::string& value);


private:
    template <typename T>
    T _get(const std::string& key, T defaultValue = T{}) const;

    /**
     * Return a string value for the given key. If not found, the default value is returned.
     */
    std::string _getString(const std::string& key, const std::string& defaultValue) const;

    /**
     * Return an int value for the given key. If not found, then a zero value is returned.
     */
    long long _getInt(const std::string& key, long long defaultValue = 0) const;

    /**
     * Return a double value for the given key. If not found, then a zero value is returned.
     */
    long double _getDouble(const std::string& key, long double defaultValue = 0.0) const;

    /**
     * Return a bool value for the given key. If not found, then false is returned.
     */
    bool _getBool(const std::string& key, bool defaultValue = false) const;

};

template <typename T, typename U>
U UserPreferences::get(const std::string& key, T defaultValue) const {
    return _get<U>(key, std::move(defaultValue));
}

inline bool UserPreferences::has(const std::string& key) const { return UserPreferencesImpl::has(key); }
inline void UserPreferences::unset(const std::string& key) { return UserPreferencesImpl::unset(key); }

template <>
inline std::string UserPreferences::_get<std::string>(const std::string& key, std::string defaultValue) const {
    return _getString(key, std::move(defaultValue));
}

template <>
inline bool UserPreferences::_get<bool>(const std::string& key, bool defaultValue) const {
    return _getBool(key, defaultValue);
}

template <>
inline char UserPreferences::_get<char>(const std::string& key, char defaultValue) const {
    auto result = _getString(key, std::string{defaultValue, 1});
    if (result.empty()) {
        return defaultValue;
    } else {
        return result.front();
    }
}

template <>
inline unsigned char UserPreferences::_get<unsigned char>(const std::string& key, unsigned char defaultValue) const {
    return static_cast<unsigned char>(_getInt(key, defaultValue));
}

template <>
inline short UserPreferences::_get<short>(const std::string& key, short defaultValue) const {
    return static_cast<short>(_getInt(key, defaultValue));
}

template <>
inline int UserPreferences::_get<int>(const std::string& key, int defaultValue) const {
    return static_cast<int>(_getInt(key, defaultValue));
}

template <>
inline long UserPreferences::_get<long>(const std::string& key, long defaultValue) const {
    return static_cast<long>(_getInt(key, defaultValue));
}

template <>
inline long long UserPreferences::_get<long long>(const std::string& key, long long defaultValue) const {
    return _getInt(key, defaultValue);
}

template <>
inline unsigned short UserPreferences::_get<unsigned short>(const std::string& key, unsigned short defaultValue) const {
    return static_cast<unsigned short>(_getInt(key, defaultValue));
}

template <>
inline unsigned int UserPreferences::_get<unsigned int>(const std::string& key, unsigned int defaultValue) const {
    return static_cast<unsigned int>(_getInt(key, defaultValue));
}

template <>
inline unsigned long UserPreferences::_get<unsigned long>(const std::string& key, unsigned long defaultValue) const {
    return static_cast<unsigned long>(_getInt(key, defaultValue));
}

template <>
inline unsigned long long UserPreferences::_get<unsigned long long>(const std::string& key,
                                                                   unsigned long long defaultValue) const {
    return _getInt(key, defaultValue);
}

template <>
inline float UserPreferences::_get<float>(const std::string& key, float defaultValue) const {
    return static_cast<float>(_getDouble(key, defaultValue));
}

template <>
inline double UserPreferences::_get<double>(const std::string& key, double defaultValue) const {
    return static_cast<double>(_getDouble(key, defaultValue));
}

template <>
inline long double UserPreferences::_get<long double>(const std::string& key, long double defaultValue) const {
    return _getDouble(key, defaultValue);
}

inline std::string UserPreferences::_getString(const std::string& key, const std::string& defaultValue) const {
    if (!has(key)) {
        return defaultValue;
    }
    return UserPreferencesImpl::getString(key);
}

inline long long UserPreferences::_getInt(const std::string& key, long long defaultValue) const {
    if (!has(key)) {
        return defaultValue;
    }
    return UserPreferencesImpl::getInt(key);
}

inline long double UserPreferences::_getDouble(const std::string& key, long double defaultValue) const {
    if (!has(key)) {
        return defaultValue;
    }
    return UserPreferencesImpl::getDouble(key);
}

inline bool UserPreferences::_getBool(const std::string& key, bool defaultValue) const {
    if (!has(key)) {
        return defaultValue;
    }
    return UserPreferencesImpl::getBool(key);
}

inline void UserPreferences::set(const std::string& key, const std::string& value) {
    return UserPreferencesImpl::setString(key, value);
}

inline void UserPreferences::set(const std::string& key, long long value) {
    return UserPreferencesImpl::setInt(key, value);
}

inline void UserPreferences::set(const std::string& key, unsigned long long value) {
    return UserPreferencesImpl::setInt(key, value);
}

inline void UserPreferences::set(const std::string& key, long value) {
    return UserPreferencesImpl::setInt(key, value);
}

inline void UserPreferences::set(const std::string& key, unsigned long value) {
    return UserPreferencesImpl::setInt(key, value);
}

inline void UserPreferences::set(const std::string& key, int value) {
    return UserPreferencesImpl::setInt(key, value);
}

inline void UserPreferences::set(const std::string& key, unsigned value) {
    return UserPreferencesImpl::setInt(key, value);
}

inline void UserPreferences::set(const std::string& key, float value) {
    return UserPreferencesImpl::setDouble(key, value);
}

inline void UserPreferences::set(const std::string& key, double value) {
    return UserPreferencesImpl::setDouble(key, value);
}

inline void UserPreferences::set(const std::string& key, long double value) {
    return UserPreferencesImpl::setDouble(key, value);
}

inline void UserPreferences::set(const std::string& key, bool value) {
    return UserPreferencesImpl::setBool(key, value);
}

} // namespace scraps
