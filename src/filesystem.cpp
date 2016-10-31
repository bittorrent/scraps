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
#include "scraps/filesystem.h"

#include <cstring>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

namespace scraps {

bool CreateDirectory(std::string path, bool createParents) {
    if (path.size() > 1 && path.back() == '/') { path.pop_back(); }
    if (createParents) {
        if (auto parent = ParentDirectory(path)) {
            if (!IsDirectory(*parent)) {
                CreateDirectory(*parent, true);
            }
        }
    }
    return !mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

bool RemoveDirectory(const std::string& path) {
    return !rmdir(path.c_str());
}

bool IsDirectory(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info)) { return false; }
    return S_ISDIR(info.st_mode);
}

bool IterateDirectory(const std::string& path, const std::function<void(const char* name, bool isFile, bool isDirectory)>& callback) {
    auto dir = opendir(path.c_str());
    if (!dir) { return false; }
    while (auto entry = readdir(dir)) {
        callback(entry->d_name, entry->d_type & DT_REG, entry->d_type & DT_DIR);
    }
    closedir(dir);
    return true;
}

stdts::optional<std::string> ParentDirectory(const std::string& path) {
    std::string ret{path};
    while (!ret.empty() && ret.back() != '/') { ret.pop_back(); }
    if (ret.empty()) { return {}; }
    if (ret.size() > 1) {
        ret.pop_back();
    }
    if (ret == path) { return {}; }
    return ret;
}

} // namespace scraps
