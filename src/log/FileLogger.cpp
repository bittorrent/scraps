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
#include <scraps/log/FileLogger.h>

#include <scraps/filesystem.h>

#if SCRAPS_APPLE
#import <Foundation/Foundation.h>
#endif

namespace scraps::log {

FileLogger::FileLogger(const char* filePath, size_t rotateSize, size_t maxFiles)
    : FileLogger{nullptr, filePath, rotateSize, maxFiles}
{
}

FileLogger::FileLogger(std::shared_ptr<FormatterInterface> formatter,
                       const char* filePath,
                       size_t rotateSize,
                       size_t maxFiles)
    : FormattedLogger{formatter}
    , _rotateSize{rotateSize}
    , _filePath{filePath}
    , _maxFiles{maxFiles}
{
    if (!(_file = fopen(filePath, "a+"))) {
        SCRAPS_LOGF_ERROR("couldn't open %s for logging", _filePath);
    }
}

FileLogger::~FileLogger() { fclose(_file); }

void FileLogger::write(LogLevel level, const std::string& message) {
    if (!_file) { return; }

    std::lock_guard<std::mutex> lock{_mutex};

    auto pos = ftell(_file);
    if (_rotateSize && (pos == -1 || static_cast<size_t>(pos) >= _rotateSize)) {
        _rotate();
    }

    fputs(message.c_str(), _file);
    fputc('\n', _file);
    fflush(_file);
}

std::string FileLogger::DefaultLogPath(const std::string& appName) {
#if SCRAPS_MACOS
    return [[NSString stringWithFormat:@"%@/Library/Logs/%s/", NSHomeDirectory(), appName.c_str()] UTF8String];
#elif SCRAPS_IOS || SCRAPS_TVOS
    return [NSTemporaryDirectory() UTF8String];
#elif SCRAPS_LINUX
    return "/var/log/" + appName + "/";
#elif SCRAPS_ANDROID
    return "";
#elif SCRAPS_WINDOWS
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path))) {
        return std::string(path) + "\\" + appName + "\\";
    }

    return "";
#endif
}

#if SCRAPS_APPLE || SCRAPS_LINUX || SCRAPS_WINDOWS
std::shared_ptr<FileLogger> FileLogger::CreateApplicationLogger(std::shared_ptr<FormatterInterface> formatter,
                                                                const std::string& appName,
                                                                size_t rotateSize,
                                                                size_t maxFiles) {
    assert(!appName.empty());
    std::string logPath = FileLogger::DefaultLogPath(appName);
    CreateDirectory(logPath, true);
    if (IsDirectory(logPath)) {
        if (logPath.back() != '/') {
            logPath += '/';
        }
        logPath += appName + ".log";
        return std::make_shared<FileLogger>(std::move(formatter), logPath.c_str(), rotateSize, maxFiles);
    }
    return nullptr;
}
#endif


void FileLogger::_rotate() {
    fclose(_file);
    for (size_t i = _maxFiles - 1; i >= 1; --i) {
        auto path = _filePath + '.' + _numberString(i);
        if (i == _maxFiles - 1) {
            remove(path.c_str());
        } else {
            rename(path.c_str(), (_filePath + '.' + _numberString(i + 1)).c_str());
        }
    }
    rename(_filePath.c_str(), (_filePath + '.' + _numberString(1)).c_str());
    _file = fopen(_filePath.c_str(), "w+");
}

std::string FileLogger::_numberString(size_t n) {
    int precision = std::log10((double)(_maxFiles - 1)) + 1;
    return Formatf(Formatf("%%.%du", precision).c_str(), n);
}

} // namespace scraps::log
