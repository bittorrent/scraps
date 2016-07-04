#pragma once

#include "scraps/config.h"

#include <string>
#include <unordered_map>

namespace scraps {

/**
* @return mapping of library names to license text
*/
std::unordered_map<std::string, std::string> ThirdPartyLicenses();

} // namespace scraps
