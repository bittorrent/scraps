#include "gtest/gtest.h"

#include "scraps/net/TCPAcceptor.h"
#include "scraps/net/TCPService.h"

using namespace scraps;
using namespace scraps::net;

namespace {
    size_t gNumConnections = 0;
}

struct TestConnection {
    TestConnection(int socket) { ++gNumConnections; }
    void run() {}
    void cancel() {}
};

struct TestDelegate : public TCPServiceDelegate {
    virtual void tcpServiceConnectionEstablished(TCPService::ConnectionId connectionId) override { state = kConnected; }
    virtual void tcpServiceConnectionFailed(TCPService::ConnectionId connectionId) override { state = kFailed; }
    virtual void tcpServiceConnectionClosed(TCPService::ConnectionId connectionId) override { state = kClosed; }

    enum State {
        kUnknown,
        kConnected,
        kClosed,
        kFailed,
    };

    std::atomic<State> state{kUnknown};
};

void connectTest(uint16_t port, bool expectSuccess = true) {
    TestDelegate delegate;
    TCPService connector{&delegate};
    connector.start();
    auto id = connector.connect("127.0.0.1", port);

    connector.send(id, "hello world"); // establish connection

    while (delegate.state == TestDelegate::kUnknown) {
        std::this_thread::sleep_for(10ms);
    }

    EXPECT_EQ(delegate.state, expectSuccess ? TestDelegate::kConnected : TestDelegate::kFailed);
    connector.stop();
    connector.wait();
    EXPECT_EQ(delegate.state, TestDelegate::kClosed);
}

TEST(TCPAcceptor, normalOperation) {
    {
        TCPAcceptor<TestConnection> acceptor;
        auto success = acceptor.start(Address::from_string("127.0.0.1"), 6543);
        ASSERT_TRUE(success);

        connectTest(6543);
        EXPECT_EQ(gNumConnections, 1);

        connectTest(6543);
        EXPECT_EQ(gNumConnections, 2);

        acceptor.stop();

        gNumConnections = 0;
    }

    {
        TCPAcceptor<TestConnection> acceptor;
        auto success = acceptor.start(Address::from_string("127.0.0.1"), 7654);
        ASSERT_TRUE(success);

        connectTest(7654);
        EXPECT_EQ(gNumConnections, 1);

        connectTest(7654);
        EXPECT_EQ(gNumConnections, 2);

        acceptor.stop();
    }
}
