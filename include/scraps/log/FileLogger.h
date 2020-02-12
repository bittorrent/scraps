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

#include <scraps/config.h>

#include <scraps/log/FormattedLogger.h>

#include <cstdio>
#include <mutex>
#include <string>

namespace scraps::log {

/**
* The FileLogger class logs messages to a file.
*/
class FileLogger : public FormattedLogger {
public:
    FileLogger(const char* filePath, size_t rotateSize = 0, size_t maxFiles = 5);
    FileLogger(std::shared_ptr<FormatterInterface> formatter,
               const char* filePath,
               size_t rotateSize = 0,
               size_t maxFiles = 5);

    virtual ~FileLogger();

    virtual void write(Level level, const std::string& formattedMessage) override;

    /**
    * Returns the default logging path for the current system.
    *
    * Mac OS X: ~/Library/Logs/<appName>
    * iOS: %app%/tmp
    * Linux: /var/log/<appName>
    * Windows: %appdata%/<appName>
    */
    static std::string DefaultLogPath(const std::string& appName);

    /**
    * Create the most appropriate file logger for the system.
    *
    * iOS/Mac OS X: FileLogger
    * Linux: FileLogger
    * Windows: FileLogger
    *
    * For file log locations, see DefaultLogPath.
    */
#if SCRAPS_APPLE || SCRAPS_LINUX || SCRAPS_WINDOWS
    static std::shared_ptr<FileLogger> CreateApplicationLogger(std::shared_ptr<FormatterInterface> formatter,
                                                               const std::string& appName,
                                                               size_t rotateSize,
                                                               size_t maxFiles);
#endif

private:
    void _rotate();
    std::string _numberString(size_t n);

    size_t _rotateSize;
    std::string _filePath;
    size_t _maxFiles;
    FILE* _file;
    std::mutex _mutex;
};

} // namespace scraps::log
