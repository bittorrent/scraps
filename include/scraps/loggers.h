#pragma once

#include "scraps/config.h"

#include "scraps/CircularBuffer.h"
#include "scraps/logging.h"
#include "scraps/thread.h"
#include "scraps/utility.h"
#include "scraps/net/UDPSocket.h"

#include <mutex>
#include <stdio.h>
#include <queue>
#include <condition_variable>

namespace scraps {

/**
* The StandardLogger class logs messages to stdout and stderr.
*/
class StandardLogger : public Logger {
public:
    virtual void log(LogLevel level, const std::string& message) override;
    virtual ~StandardLogger() {}

private:
    std::mutex _mutex;
};

#if __ANDROID__
/**
* The AndroidLogger class logs messages to the Android log.
*/
class AndroidLogger : public Logger {
public:
    virtual void log(LogLevel level, const std::string& message) override;
    virtual ~AndroidLogger() {}

private:
    std::mutex _mutex;
};
#endif

/**
* The FileLogger class logs messages to a file.
*/
class FileLogger : public Logger {
public:
    FileLogger(const char* filePath, size_t rotateSize = 0, size_t maxFiles = 5);
    virtual ~FileLogger();

    virtual void log(LogLevel level, const std::string& message) override;

    /**
    * Returns the default logging path for the current system.
    *
    * Mac OS X: ~/Library/Logs/<appName>
    * iOS: %app%/tmp
    * Linux: /var/log/<appName>
    * Windows: %appdata%/<appName>
    */
    static std::string DefaultLogPath(const std::string& appName);

private:
    FileLogger(const FileLogger& other) {}
    FileLogger& operator=(const FileLogger& other) { return *this; }

    void _rotate();
    std::string _numberString(size_t n);

    size_t _rotateSize;
    std::string _filePath;
    size_t _maxFiles;
    FILE* _file;
    std::mutex _mutex;
};

/**
* The LoggerLogger class logs messages to other loggers.
*/
class LoggerLogger : public Logger {
public:
    template <typename... Loggers>
    LoggerLogger(Loggers&&... loggers) {
        _addLoggers(std::forward<Loggers>(loggers)...);
    }

    virtual ~LoggerLogger() {}

    virtual void log(LogLevel level, std::chrono::system_clock::time_point time, const char* file, unsigned int line, const std::string& message) override;

private:
    void _addLoggers() {}

    template <typename Next, typename... Loggers>
    void _addLoggers(Next&& next, Loggers&&... loggers) {
        _loggers.push_back(next);
        _addLoggers(loggers...);
    }

    std::vector<std::shared_ptr<Logger>> _loggers;
};

/**
* The BufferLogger class logs messages to a buffer and passes them on in batches.
* Each batch is spliced together via newline and passed on at the highest level in the batch.
*/
class BufferLogger : public Logger {
public:
    typedef decltype(std::chrono::steady_clock::now().time_since_epoch()) DurationType;

    BufferLogger(std::shared_ptr<Logger> logger, const DurationType& flushInterval, size_t flushSize);
    virtual ~BufferLogger();

    virtual void log(LogLevel level, const std::string& message) override;

private:
    DurationType _flushInterval;
    size_t _flushSize;
    std::shared_ptr<Logger> _logger;
    DurationType _lastFlush;
    LogLevel _logLevel;
    std::string _buffer;
    std::mutex _mutex;
};

/**
* Forwards logs asyncronously.
*/
class AsyncLogger : public Logger {
public:
    AsyncLogger(std::shared_ptr<Logger> logger);
    virtual ~AsyncLogger();

    virtual void log(LogLevel level, const std::string& message) override;

private:
    void _run();

    std::shared_ptr<Logger> _logger;
    std::thread _worker;
    std::mutex _mutex;
    std::condition_variable _condition;
    bool _shouldReturn;
    std::queue<std::pair<LogLevel, std::string>> _backlog;
};

/**
* Filters logs.
*/
class FilterLogger : public Logger {
public:
    FilterLogger(std::shared_ptr<Logger> logger, LogLevel minimumLevel) : _logger(logger), _minimumLevel(minimumLevel) {}
    virtual ~FilterLogger() {}

    virtual void log(LogLevel level, const std::string& message) override;

private:
    std::shared_ptr<Logger> _logger;
    LogLevel _minimumLevel;
};

/**
* The CircularBufferLogger class logs messages to a circular buffer.
*/
class CircularBufferLogger : public Logger {
public:
    CircularBufferLogger(size_t capacity) : _buffer(capacity) {}
    virtual ~CircularBufferLogger() {}

    virtual void log(LogLevel level, const std::string& message) override;

    std::string contents() const;

private:
    mutable std::mutex _mutex;
    CircularBuffer<char> _buffer;
};

/**
* Sends DogStatsD events for log messages.
*/
class DogStatsDLogger : public Logger {
public:
    DogStatsDLogger(net::Endpoint endpoint, LogLevel level = LogLevel::kWarning);

    virtual void log(LogLevel level, std::chrono::system_clock::time_point time, const char* file, unsigned int line, const std::string& message) override;

private:
    std::mutex _mutex;
    const net::Endpoint _endpoint;
    const LogLevel _level;
    std::unique_ptr<net::UDPSocket> _socket;
};

} // namespace scraps
