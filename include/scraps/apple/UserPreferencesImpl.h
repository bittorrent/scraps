#pragma once

#include "scraps/config.h"

#if SCRAPS_APPLE

#include "scraps/UserPreferences.h"

#import <Foundation/Foundation.h>

namespace scraps {

/**
* Not thread-safe.
*
* Store user preferences in an OS-specific way. (e.g. UserDefaults for Mac)
*/
class UserPreferencesImpl : public UserPreferences {
public:
    UserPreferencesImpl();
    UserPreferencesImpl(const std::string& suiteName);

    UserPreferencesImpl(const UserPreferencesImpl&) = delete;
    UserPreferencesImpl& operator=(const UserPreferencesImpl&) = delete;
    UserPreferencesImpl(UserPreferencesImpl&&) = delete;
    UserPreferencesImpl& operator=(UserPreferencesImpl&&) = delete;

    virtual bool has(const std::string& key) const override;
    virtual void unset(const std::string& key) override;

    virtual void set(const std::string& key, const std::string& value) override;
    virtual void set(const std::string& key, int value) override;
    virtual void set(const std::string& key, float value) override;
    virtual void set(const std::string& key, bool value) override;

private:
    virtual std::string getString(const std::string& key) const override;
    virtual int getInt(const std::string& key) const override;
    virtual float getFloat(const std::string& key) const override;
    virtual bool getBool(const std::string& key) const override;

    NSUserDefaults* _userDefaults = nil;
};

} // namespace scraps

#endif // SCRAPS_APPLE
