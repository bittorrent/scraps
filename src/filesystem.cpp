#include "scraps/filesystem.h"

#include <cstring>

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
