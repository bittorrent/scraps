#include "scraps/Application.h"

#include "scraps/ArgumentParser.h"
#include "scraps/utility.h"
#include "scraps/loggers.h"

#include <gsl.h>

#include <iostream>

#if SCRAPS_APPLE
#import <Foundation/Foundation.h>
#endif

namespace scraps {

Application* Application::g_instance = nullptr;

Application::Application() :
    _appVersion{kVersion}
{
    SCRAPS_ASSERT(g_instance == nullptr);
    g_instance = this;
}

Application::~Application() {
    g_instance = nullptr;
}

bool Application::processArguments(int argc, const char* argv[]) {
    if (argc == 0) {
        return false;
    }

    _arguments.resize(argc);
    _argv.resize(argc);
    for (int i = 0; i < argc; ++i) {
        _arguments[i] = argv[i];
        _argv[i] = _arguments[i].c_str();
    }

    _appName = Basename(_arguments[0]);

#if SCRAPS_APPLE
    if (NSBundle* bundle = [NSBundle mainBundle]) {
        if (NSDictionary* info = [bundle infoDictionary]) {
            if (NSString* bundleName = [info objectForKey:@"CFBundleName"]) {
                _appName = [bundleName UTF8String];
            }
        }
    }
#endif

    _argumentParser = std::make_shared<ArgumentParser>(_appName);
    prepareArguments(*_argumentParser);
    return _argumentParser->parse(argc, argv);
}

void Application::prepareArguments(ArgumentParser& argParser) {
    argParser.addFlag("help",         "show options",             [&]{ setFlag("help"); });
    argParser.addFlag("version",      "show application version", [&]{ setFlag("version"); });
    argParser.addFlag("debug",        "enable debug logging",     [&]{ setFlag("debug"); });
    argParser.addArgument("log-file", "log file name", &_logFile);
}

std::shared_ptr<Logger> Application::createLogger() {
    if (_logFile.empty()) {
#if SCRAPS_ANDROID
        return std::make_shared<AndroidLogger>();
#else
        return std::make_shared<StandardLogger>();
#endif
    } else {
        return std::make_shared<FileLogger>(_logFile.c_str());
    }
}

void Application::initializeLogger() {
    auto logger = createLogger();

#if !NDEBUG
    SetLogger(logger);
    SetLogLevel(LogLevel::kDebug);
#else
    SetLogger(std::make_shared<AsyncLogger>(logger));
    SetLogLevel(hasFlag("debug") ? LogLevel::kDebug : LogLevel::kInfo);
#endif
}

bool Application::handleCommonFlags() const {
    if (numFlags() == 0) {
        return false;
    } else if (hasFlag("version")) {
        writeApplicationVersion(*_outStream);
        return true;
    } else if (hasFlag("help")) {
        writeHelp(*_outStream);
        return true;
    }

    return false;
}

int Application::run() {
    initialize();
    auto _ = gsl::finally([=]{ shutdown(); });
    return main();
}

std::string Application::applicationPath() const {
    if (!_arguments.empty()) {
        return Dirname(_arguments[0]);
    }
#if SCRAPS_APPLE
    if (NSBundle* bundle = [NSBundle mainBundle]) {
        if (NSString* path = [bundle executablePath]) {
            return [path UTF8String];
        }
    }
#endif
    return "";
}

void Application::writeHelp(std::ostream& ostr) const {
    if (_argumentParser) {
        _argumentParser->help(ostr);
    }
}

void Application::writeApplicationVersion(std::ostream& ostr) const {
    ostr << applicationName() << ' ' << applicationVersion().toString() << std::endl;
}

} // namespace scraps
