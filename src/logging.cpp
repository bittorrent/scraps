#include "scraps/logging.h"

#include "scraps/loggers.h"

#include <boost/filesystem.hpp>

#if SCRAPS_APPLE
#import <Foundation/Foundation.h>
#endif

namespace scraps {

std::shared_ptr<Logger> _gLogger;

std::atomic<LogLevel> _gLogLevel{LogLevel::kInfo};

std::shared_ptr<Logger> CreateDefaultFileLogger(const std::string& appName, size_t rotateSize, size_t maxFiles) {
#if SCRAPS_APPLE || SCRAPS_LINUX || SCRAPS_WINDOWS
    SCRAPS_ASSERT(!appName.empty());
    std::string logPath = FileLogger::DefaultLogPath(appName);
    boost::system::error_code error;
    boost::filesystem::create_directories(logPath, error);
    if (boost::filesystem::exists(logPath)) {
        logPath += appName + ".log";
        return std::make_shared<FileLogger>(logPath.c_str(), rotateSize, maxFiles);
    }
#endif
    return nullptr;
}

} // namespace scraps
