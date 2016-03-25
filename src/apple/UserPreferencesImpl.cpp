#include "scraps/platform.h"

#if SCRAPS_APPLE

#include "scraps/UserPreferences.h"

#import <Foundation/Foundation.h>

namespace scraps {

UserPreferencesImpl::UserPreferencesImpl() : _userDefaults{[NSUserDefaults standardUserDefaults]} {}

UserPreferencesImpl::UserPreferencesImpl(const std::string& suiteName) {
    if (suiteName.empty()) {
        _userDefaults = [NSUserDefaults standardUserDefaults];
    } else {
        NSString* nsSuiteName = [NSString stringWithUTF8String:suiteName.c_str()];
        _userDefaults = [[NSUserDefaults alloc] initWithSuiteName:nsSuiteName];
    }
}

bool UserPreferencesImpl::has(const std::string& key) const {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    id nsValue = [_userDefaults objectForKey:nsKey];
    return nsValue != nil;
}

std::string UserPreferencesImpl::getString(const std::string& key, const std::string& defaultValue) const {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    NSString* nsValue = [_userDefaults objectForKey:nsKey];
    return nsValue ? [nsValue UTF8String] : defaultValue;
}

void UserPreferencesImpl::setString(const std::string& key, const std::string& value) {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    NSString* nsValue = [NSString stringWithUTF8String:value.c_str()];
    [_userDefaults setObject:nsValue forKey:nsKey];
}

int64_t UserPreferencesImpl::getInt(const std::string& key) const {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    return [_userDefaults integerForKey:nsKey];
}

void UserPreferencesImpl::setInt(const std::string& key, int64_t value) {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    [_userDefaults setInteger:value forKey:nsKey];
}

double UserPreferencesImpl::getDouble(const std::string& key) const {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    return [_userDefaults doubleForKey:nsKey];
}

void UserPreferencesImpl::setDouble(const std::string& key, double value) {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    [_userDefaults setDouble:value forKey:nsKey];
}

bool UserPreferencesImpl::getBool(const std::string& key) const {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    return [_userDefaults boolForKey:nsKey];
}

void UserPreferencesImpl::setBool(const std::string& key, bool value) {
    NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
    [_userDefaults setBool:value forKey:nsKey];
}

} // namespace scraps

#endif
