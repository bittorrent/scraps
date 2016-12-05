/**
* Copyright 2016 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
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
* @return true if the given path exists and could be opened
*/
bool IterateDirectory(const std::string& path, const std::function<void(const char* name, bool isFile, bool isDirectory)>& callback);

/**
* Returns the parent directory of a path. This is a simple string operation, and functions similarly
* to Python's os.path.dirname.
*
* @return the parent directory if there is one
*/
stdts::optional<std::string> ParentDirectory(const std::string& path);

} // namespace scraps
