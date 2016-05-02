#include "scraps/UserPreferences.h"

namespace scraps {

UserPreferences::UserPreferences(std::string suiteName) : UserPreferencesImpl{std::move(suiteName)} {}

} // namespace scraps
