#pragma once

#include "scraps/config.h"
#include "scraps/format.h"

#include <atomic>
#include <ctime>
#include <functional>

namespace scraps {

/**
* LogLevel values are ordered according to their severity.
*/
enum class LogLevel {
    kDebug,
    kInfo,
    kWarning,
    kError,
};

extern std::atomic<LogLevel> _gLogLevel;

inline constexpr const char* LogLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::kDebug: return "DEBUG";
        case LogLevel::kInfo: return "INFO";
        case LogLevel::kWarning: return "WARNING";
        case LogLevel::kError: return "ERROR";
    }
    return "???";
}

/**
* Subclass this to create loggers.
*/
class Logger {
public:
    virtual ~Logger() {}

    /**
    * This implementation should be thread-safe.
    */
    virtual void log(LogLevel level, std::chrono::system_clock::time_point time, const char* file, unsigned int line, const std::string& message) {
        char buffer[sizeof("0000-00-00 00:00:00")] = {};
        auto d = time.time_since_epoch();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(d - std::chrono::duration_cast<std::chrono::seconds>(d));
        auto timet = std::chrono::system_clock::to_time_t(time);
        strftime(buffer, sizeof(buffer), "%F %T", gmtime(&timet));
        log(level, Formatf("[%s.%03d] %s %s:%u %s", buffer, milliseconds.count(), LogLevelString(level), file, line, message));
    }

    /**
    * This implementation should be thread-safe.
    */
    virtual void log(LogLevel level, const std::string& message) {
        assert(false);
    }
};

extern std::shared_ptr<Logger> _gLogger;

/**
* Create the most appropriate file logger for the system.
*
* Android: creates AndroidLogger.
* iOS/Mac OS X: FileLogger
* Linux: FileLogger
* Windows: FileLogger
*
* For file log locations, see FileLogger::DefaultLogPath.
*
*/
std::shared_ptr<Logger> CreateDefaultFileLogger(const std::string& appName,
                                            size_t rotateSize = 1024 * 1024,
                                            size_t maxFiles = 5);

/**
* Returns the current logger.
*/
inline std::shared_ptr<Logger> CurrentLogger() { return _gLogger; }

/**
* Set the current logger.
*/
inline void SetLogger(std::shared_ptr<Logger> logger) { _gLogger = logger; }

/**
* Returns the current log level.
*/
inline LogLevel CurrentLogLevel() { return _gLogLevel; }

/**
* Sets the log level (the minimum level to log).
*/
inline void SetLogLevel(LogLevel level) { _gLogLevel = level; }

namespace detail {

inline void LogImpl(LogLevel level, std::chrono::system_clock::time_point time, const char* file, unsigned int line, const std::string& message) {
    static std::recursive_mutex mutex;
    std::lock_guard<std::recursive_mutex> lock{mutex};
    static bool isLogging = false;
    if (isLogging) { return; }
    if (auto logger = _gLogger) {
        isLogging = true;
        logger->log(level, time, file, line, message);
        isLogging = false;
    }
}

} // namepsace detail

/**
* Formats a log message and sends it to the current logger.
*/
template <typename... Args>
void Logf(LogLevel level, const char* file, unsigned int line, const char* format, Args&&... args) {
    if (level < _gLogLevel) { return; }
    detail::LogImpl(level, std::chrono::system_clock::now(), file, line, Formatf(format, std::forward<Args>(args)...));
}

/**
* Formats a log message and sends it to the current logger.
*/
template <typename... Args>
void Log(LogLevel level, const char* file, unsigned int line, const char* format, Args&&... args) {
    if (level < _gLogLevel) { return; }
    detail::LogImpl(level, std::chrono::system_clock::now(), file, line, Format(format, std::forward<Args>(args)...));
}

#define SCRAPS_LOG(LEVEL, ...)   scraps::Log(LEVEL, __FILE__, __LINE__, __VA_ARGS__)
#define SCRAPS_LOG_DEBUG(...)    scraps::Log(scraps::LogLevel::kDebug,    __FILE__, __LINE__, __VA_ARGS__)
#define SCRAPS_LOGF_DEBUG(...)   scraps::Logf(scraps::LogLevel::kDebug,   __FILE__, __LINE__, __VA_ARGS__)
#define SCRAPS_LOG_INFO(...)     scraps::Log(scraps::LogLevel::kInfo,     __FILE__, __LINE__, __VA_ARGS__)
#define SCRAPS_LOGF_INFO(...)    scraps::Logf(scraps::LogLevel::kInfo,    __FILE__, __LINE__, __VA_ARGS__)
#define SCRAPS_LOG_WARNING(...)  scraps::Log(scraps::LogLevel::kWarning,  __FILE__, __LINE__, __VA_ARGS__)
#define SCRAPS_LOGF_WARNING(...) scraps::Logf(scraps::LogLevel::kWarning, __FILE__, __LINE__, __VA_ARGS__)
#define SCRAPS_LOG_ERROR(...)    scraps::Log(scraps::LogLevel::kError,    __FILE__, __LINE__, __VA_ARGS__)
#define SCRAPS_LOGF_ERROR(...)   scraps::Logf(scraps::LogLevel::kError,   __FILE__, __LINE__, __VA_ARGS__)

} // namespace scraps
