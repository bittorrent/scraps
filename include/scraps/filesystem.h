#pragma once

#include "scraps/config.h"
#include "stdts/optional.h"

#include <string>

// TODO: when we see wide enough C++17 support, these will be replaced by <filesystem>

namespace scraps {

/**
* @return true on success
*/
bool CreateDirectory(std::string path, bool createParents=false);

/**
* @return true on success
*/
bool RemoveDirectory(const std::string& path);

/**
* @return true if the given path exists and is a directory
*/
bool IsDirectory(const std::string& path);

/**
* Returns the parent directory of a path. This is a simple string operation, and functions similarly
* to Python's os.path.dirname.
*
* @return the parent directory if there is one
*/
stdts::optional<std::string> ParentDirectory(const std::string& path);

} // namespace scraps
