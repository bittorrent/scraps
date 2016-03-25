#pragma once

#include "scraps/config.h"

#include "scraps/Version.h"

#include <unordered_set>
#include <iostream>

namespace scraps {

class ArgumentParser;
class Logger;

/**
 * Provide a structured way to initialize and shutdown an application.
 *
 * int main(int argc, const char* argv[]) {
 *     Application app;
 *     if (app.processArguments(argc, argv)) {
 *         return app.run();
 *     }
 *     app.writeHelp(std::cout);
 *     return 1;
 * }
 *
 * Inspired from https://github.com/pocoproject/poco/blob/develop/Util/include/Poco/Util/Application.h.
 */
class Application {
public:
    Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    virtual ~Application();

    static Application& Instance() { return *g_instance; }

    /**
     * Process command line arguments. Results are preserved for later use via
     * argc() and argv().
     *
     * Returns true if successful, false otherwise.
     */
    virtual bool processArguments(int argc, const char* argv[]);

    /**
     * Calls
     * - initialize()
     * - main()
     * - shutdown()
     *
     * shutdown() will be called even if there is an exception thrown in main().
     */
    virtual int run();

    int argc() const { return static_cast<int>(_argv.size()); }
    const char** argv() const { return const_cast<const char**>(&_argv[0]); }

    const std::string& applicationName() const { return _appName; }
    const Version& applicationVersion() const { return _appVersion; }
    std::string applicationPath() const;

    bool hasFlag(const std::string& key) const { return _flags.find(key) != _flags.end(); }
    void clearFlag(const std::string& key) { _flags.erase(key); }
    void setFlag(const std::string& key) { _flags.insert(key); }
    size_t numFlags() const { return _flags.size(); }

    /**
     * Write the help for the command line arguments.
     */
    void writeHelp(std::ostream& ostr) const;

    /**
     * Output the application name and version.
     */
    void writeApplicationVersion(std::ostream& ostr) const;

protected:
    /**
     * Perform any application-wide initialization.
     */
    virtual void initialize() {
        initializeLogger();
    }

    /**
     * Gracefully shutdown application components.
     */
    virtual void shutdown() {}

    /**
     * Runs the application.
     */
    virtual int main() = 0;

    /**
     * Handles the common command line flags like --help, --version, etc.
     *
     * Returns true if the command was handled, false otherwise.
     */
    virtual bool handleCommonFlags() const;

    /**
     * Create a default logger.
     */
    virtual std::shared_ptr<Logger> createLogger();

    /**
     * Initialize a logger with the default settings.
     */
    virtual void initializeLogger();

    /**
     * Add arguments to ArgumentParser.
     */
    virtual void prepareArguments(ArgumentParser& argParser);

    /**
     * Sets the application name.
     */
    void setApplicationName(const std::string& value) { _appName = value; }

    /**
     * Set the application version.
     */
    void setApplicationVersion(const Version& value) { _appVersion = value; }

    /**
     * Set the output stream that help and usage information is written to.
     */
    void setOutStream(std::ostream* stream) { _outStream = stream; }

private:
    std::vector<std::string> _arguments; // used to hold a valid const char* for _argv
    std::vector<const char*> _argv;
    std::string _appName;
    Version _appVersion;

    std::shared_ptr<ArgumentParser> _argumentParser;
    std::unordered_set<std::string> _flags;
    std::string _logFile;
    std::ostream* _outStream = &std::cout;

    static Application* g_instance;
};

} // namespace scraps
