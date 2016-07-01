#include "NullOStream.h"

#include "scraps/Application.h"

#include <gtest/gtest.h>

using namespace scraps;

class TestApplication : public Application {
public:
    bool processed = false;

    TestApplication() : Application{Version{1,0}} {
        setOutStream(&_ostream);
        _previousLogger = CurrentLogger();
        SetLogger(nullptr);
    }

    virtual ~TestApplication() override {
        SetLogger(_previousLogger);
    }
protected:
    int main() override {
        if (handleCommonFlags()) {
            return 0;
        }
        processed = true;
        return 0;
    }
private:
    NullOStream _ostream;
    std::shared_ptr<Logger> _previousLogger;
};

TEST(Application, basicOperation) {
    TestApplication app;

    std::vector<const char*> argv = {
        "/path/to/app",
        "--debug"
    };

    app.processArguments(argv.size(), &argv[0]);
    EXPECT_EQ(0, strcmp(app.argv()[0], argv[0]));

    EXPECT_EQ(0, app.run());
    EXPECT_TRUE(app.processed);

    EXPECT_EQ(1, app.numFlags());
    EXPECT_TRUE(app.hasFlag("debug"));
    app.clearFlag("debug");
    EXPECT_EQ(0, app.numFlags());

    EXPECT_FALSE(app.hasFlag("test"));
    app.setFlag("test");
    EXPECT_EQ(1, app.numFlags());
    EXPECT_TRUE(app.hasFlag("test"));
    app.clearFlag("test");
    EXPECT_EQ(0, app.numFlags());

#if SCRAPS_IOS || SCRAPS_TVOS
    EXPECT_EQ("scraps-tests-" SCRAPS_APPLE_SDK "-host", app.applicationName());
#else
    EXPECT_EQ("app", app.applicationName());
#endif
    EXPECT_EQ("/path/to", app.applicationPath());
}

TEST(Application, help) {
    TestApplication app;
    std::vector<const char*> argv = {
        "/path/to/app",
        "--help"
    };

    app.processArguments(argv.size(), &argv[0]);
    EXPECT_EQ(0, app.run());
    EXPECT_FALSE(app.processed);
}
