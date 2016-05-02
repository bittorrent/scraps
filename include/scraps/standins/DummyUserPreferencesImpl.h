#pragma once

#include "scraps/config.h"

namespace scraps {
namespace standins {

/**
* Not thread-safe.
*
* Non-functioning user preferences for testing.
*/
class DummyUserPreferencesImpl {
public:
    DummyUserPreferencesImpl();
    DummyUserPreferencesImpl(const std::string& suiteName);

    DummyUserPreferencesImpl(const DummyUserPreferencesImpl&) = delete;
    DummyUserPreferencesImpl& operator=(const DummyUserPreferencesImpl&) = delete;
    DummyUserPreferencesImpl(DummyUserPreferencesImpl&&) = delete;
    DummyUserPreferencesImpl& operator=(DummyUserPreferencesImpl&&) = delete;

    /**
     * Return true if the given key exists.
     */
    bool has(const std::string& key) const { return false; }

    /**
     * Return a string value for the given key. If not found, the default value is returned.
     */
    std::string getString(const std::string& key) const { return std::string(); }

    /**
     * Return an int value for the given key. If not found, then a zero value is returned.
     */
    long long getInt(const std::string& key) const { return 0; }

    /**
     * Return a double value for the given key. If not found, then a zero value is returned.
     */
    long double getDouble(const std::string& key) const { return 0.0; }

    /**
     * Return a bool value for the given key. If not found, then false is returned.
     */
    bool getBool(const std::string& key) const { return false; }

    /**
     * Set a string value for the given key.
     */
    void setString(const std::string& key, const std::string& value) {}

    /**
     * Set an int value for the given key.
     */
    void setInt(const std::string& key, long long value) {}

    /**
     * Set a double value for the given key.
     */
    void setDouble(const std::string& key, long double value) {}

    /**
     * Set a bool value for the given key.
     */
    void setBool(const std::string& key, bool value) {}
};


} // namespace standins
} // namespace scraps
