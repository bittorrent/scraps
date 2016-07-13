#pragma once

#include "scraps/config.h"

namespace scraps {

/**
* Not thread-safe.
*
* Store user preferences in an OS-specific way. (e.g. UserDefaults for Mac)
*/
class UserPreferences {
public:
    UserPreferences() {}
    virtual ~UserPreferences() {}

    UserPreferences(const UserPreferences&) = delete;
    UserPreferences& operator=(const UserPreferences&) = delete;
    UserPreferences(UserPreferences&&) = delete;
    UserPreferences& operator=(UserPreferences&&) = delete;

    /**
     * Return true if the given key exists.
     */
    virtual bool has(const std::string& key) const = 0;

    /**
     * Unsets the given key.
     */
    virtual void unset(const std::string& key) = 0;

    /**
     * Convert the raw type to T. If not found, the default value is returned.
     */
    template <typename T, typename U = typename std::remove_reference<typename std::remove_cv<T>::type>::type>
    U get(const std::string& key, T defaultValue = T{}) const;

    /**
     * Set a bool value for the given key.
     */
    virtual void set(const std::string& key, bool value) = 0;

    /**
     * Set an int value for the given key.
     */
    virtual void set(const std::string& key, int value) = 0;

    /**
     * Set an int value for the given key.
     */
    virtual void set(const std::string& key, unsigned value);

    /**
     * Set an int value for the given key.
     */
    virtual void set(const std::string& key, long long value);

    /**
     * Set an int value for the given key.
     */
    virtual void set(const std::string& key, unsigned long long value);

    /**
     * Set an int value for the given key.
     */
    virtual void set(const std::string& key, long value);

    /**
     * Set an int value for the given key.
     */
    virtual void set(const std::string& key, unsigned long value);

    /**
     * Set a double value for the given key.
     */
    virtual void set(const std::string& key, float value) = 0;

    /**
     * Set a double value for the given key.
     */
    virtual void set(const std::string& key, double value);

    /**
     * Set a double value for the given key.
     */
    virtual void set(const std::string& key, long double value);

    /**
     * Set a string value for the given key.
     */
    virtual void set(const std::string& key, const std::string& value) = 0;

protected:
    /**
     * Return a string value for the given key.
     */
    virtual std::string getString(const std::string& key) const = 0;

    /**
     * Return an int value for the given key.
     */
    virtual int getInt(const std::string& key) const = 0;

    /**
     * Return a bool value for the given key.
     */
    virtual bool getBool(const std::string& key) const = 0;

    /**
     * Return a float value for the given key.
     */
    virtual float getFloat(const std::string& key) const = 0;

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
    int _getInt(const std::string& key, int defaultValue = 0) const;

    /**
     * Return a double value for the given key. If not found, then a zero value is returned.
     */
    float _getFloat(const std::string& key, float defaultValue = 0.0) const;

    /**
     * Return a bool value for the given key. If not found, then false is returned.
     */
    bool _getBool(const std::string& key, bool defaultValue = false) const;

};

template <typename T, typename U>
U UserPreferences::get(const std::string& key, T defaultValue) const {
    return _get<U>(key, std::move(defaultValue));
}

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
    return _getInt(key, defaultValue);
}

template <>
inline long UserPreferences::_get<long>(const std::string& key, long defaultValue) const {
    return _getInt(key, defaultValue);
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
    return _getFloat(key, defaultValue);
}

template <>
inline double UserPreferences::_get<double>(const std::string& key, double defaultValue) const {
    return static_cast<double>(_getFloat(key, defaultValue));
}

template <>
inline long double UserPreferences::_get<long double>(const std::string& key, long double defaultValue) const {
    return static_cast<double>(_getFloat(key, defaultValue));
}

inline std::string UserPreferences::_getString(const std::string& key, const std::string& defaultValue) const {
    if (!has(key)) {
        return defaultValue;
    }
    return getString(key);
}

inline int UserPreferences::_getInt(const std::string& key, int defaultValue) const {
    if (!has(key)) {
        return defaultValue;
    }
    return getInt(key);
}

inline float UserPreferences::_getFloat(const std::string& key, float defaultValue) const {
    if (!has(key)) {
        return defaultValue;
    }
    return getFloat(key);
}

inline bool UserPreferences::_getBool(const std::string& key, bool defaultValue) const {
    if (!has(key)) {
        return defaultValue;
    }
    return getBool(key);
}

inline void UserPreferences::set(const std::string& key, long long value) {
    set(key, static_cast<int>(value));
}

inline void UserPreferences::set(const std::string& key, unsigned long long value) {
    set(key, static_cast<int>(value));
}

inline void UserPreferences::set(const std::string& key, long value) {
    set(key, static_cast<int>(value));
}

inline void UserPreferences::set(const std::string& key, unsigned long value) {
    set(key, static_cast<int>(value));
}

inline void UserPreferences::set(const std::string& key, unsigned value) {
    set(key, static_cast<int>(value));
}

inline void UserPreferences::set(const std::string& key, double value) {
    set(key, static_cast<float>(value));
}

inline void UserPreferences::set(const std::string& key, long double value) {
    set(key, static_cast<float>(value));
}

} // namespace scraps
