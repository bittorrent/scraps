#include "scraps/loggers.h"

#if SCRAPS_APPLE
#import <Foundation/Foundation.h>
#elif SCRAPS_ANDROID
#include <android/log.h>
#endif

#include <cmath>

namespace scraps {

void StandardLogger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock{_mutex};
#ifdef __ANDROID__
    auto f = stdout; // only stdout for android
#else
    auto f = level <= LogLevel::kInfo ? stdout : stderr;
#endif
    fprintf(f, "%s\n", message.c_str());
    fflush(f);
}

#if __ANDROID__
void AndroidLogger::log(LogLevel level, const std::string& message) {
    __android_log_print(level == LogLevel::kInfo ? ANDROID_LOG_INFO : ANDROID_LOG_ERROR, "live", "%s", message.c_str());
}
#endif

FileLogger::FileLogger(const char* filePath, size_t rotateSize, size_t maxFiles)
    : _rotateSize(rotateSize), _filePath(filePath), _maxFiles(maxFiles) {
    if (!(_file = fopen(filePath, "a+"))) { SCRAPS_LOGF_ERROR("couldn't open %s for logging", _filePath); }
}

FileLogger::~FileLogger() { fclose(_file); }

void FileLogger::log(LogLevel level, const std::string& message) {
    if (!_file) { return; }

    std::lock_guard<std::mutex> lock{_mutex};

    if (_rotateSize && ftell(_file) >= _rotateSize) { _rotate(); }

    fprintf(_file, "%s\n", message.c_str());
    fflush(_file);
}

std::string FileLogger::DefaultLogPath(const std::string& appName) {
#if SCRAPS_MAC_OS_X
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

void FileLogger::_rotate() {
    fclose(_file);
    for (size_t i = _maxFiles - 1; i >= 1; --i) {
        auto path = _filePath + '.' + _numberString(i);
        if (i == _maxFiles - 1) { remove(path.c_str()); } else {
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

void LoggerLogger::log(LogLevel level, std::chrono::system_clock::time_point time, const char* file, unsigned int line, const std::string& message) {
    for (auto& logger : _loggers) {
        logger->log(level, time, file, line, message);
    }
}

BufferLogger::BufferLogger(std::shared_ptr<Logger> logger, const BufferLogger::DurationType& flushInterval, size_t flushSize)
    : _flushInterval(flushInterval)
    , _flushSize(flushSize)
    , _logger(logger)
    , _lastFlush(std::chrono::steady_clock::now().time_since_epoch())
    , _logLevel{} {}

BufferLogger::~BufferLogger() {
    if (!_buffer.empty()) { _logger->log(_logLevel, _buffer.c_str()); }
}

void BufferLogger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock{_mutex};
    if (!_buffer.empty()) { _buffer.push_back('\n'); }
    _buffer.append(message);
    if (level > _logLevel) { _logLevel = level; }
    auto t = std::chrono::steady_clock::now().time_since_epoch();
    if (_buffer.size() >= _flushSize || t >= _lastFlush + _flushInterval) {
        _logger->log(_logLevel, _buffer.c_str());
        _buffer.clear();
        _lastFlush = t;
    }
}

AsyncLogger::AsyncLogger(std::shared_ptr<Logger> logger) : _logger(logger), _shouldReturn(false) {
    _worker = std::thread(&AsyncLogger::_run, this);
}

AsyncLogger::~AsyncLogger() {
    {
        std::lock_guard<std::mutex> lock{_mutex};
        _shouldReturn = true;
    }
    _condition.notify_one();
    if (_worker.joinable()) { _worker.join(); }
}

void AsyncLogger::log(LogLevel level, const std::string& message) {
    {
        std::lock_guard<std::mutex> lock{_mutex};
        _backlog.push(std::pair<LogLevel, std::string>(level, message));
    }
    _condition.notify_one();
}

void AsyncLogger::_run() {
    SetThreadName("AsyncLogger");

    std::unique_lock<std::mutex> lock{_mutex};
    while (!_shouldReturn) {
        if (_backlog.empty()) { _condition.wait(lock); }
        while (!_backlog.empty()) {
            std::pair<LogLevel, std::string> next = std::move(_backlog.front());
            _backlog.pop();
            lock.unlock(); // unlock to call out
            _logger->log(next.first, next.second);
            lock.lock();
        }
    }
}

void FilterLogger::log(LogLevel level, const std::string& message) {
    if (level < _minimumLevel) { return; }
    _logger->log(level, message);
}

void CircularBufferLogger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock{_mutex};
    _buffer.push(message.data(), message.size());
    _buffer.push('\n');
}

std::string CircularBufferLogger::contents() const {
    std::lock_guard<std::mutex> lock{_mutex};
    return std::string(_buffer.begin(), _buffer.end());
}

DogStatsDLogger::DogStatsDLogger(UDPEndpoint endpoint, LogLevel level)
    : _endpoint{std::move(endpoint)}
    , _level{level}
    , _socket{std::make_unique<UDPSocket>(UDPSocket::Protocol::kIPv4)}
{}

void DogStatsDLogger::log(LogLevel level, std::chrono::system_clock::time_point time, const char* file, unsigned int line, const std::string& message) {
    if (level < _level) { return; }
    std::lock_guard<std::mutex> lock{_mutex};

    auto text = Format("{} ({}:{})", message, file, line);
    auto data = Format("_e{{{},{}}}:{}|{}|t:{}\n", message.size(), text.size(), message, text,
        level == LogLevel::kError ? "error" : (level == LogLevel::kWarning ? "warning" : "info")
    );

    _socket->send(_endpoint, data.data(), data.size());
}

} // namespace scraps
