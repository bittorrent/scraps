#include "scraps/net/TCPService.h"

#include <gtest/gtest.h>

using namespace scraps;
using namespace scraps::net;

struct CountingDelegate : TCPServiceDelegate {
    virtual void tcpServiceConnectionEstablished(TCPService::ConnectionId connectionId) override {
        ++established;
    }

    virtual void tcpServiceConnectionFailed(TCPService::ConnectionId connectionId) override {
        ++failed;
    }

    virtual void tcpServiceConnectionReceivedData(TCPService::ConnectionId connectionId, const void* data, size_t length) override {
        ++received;
    }

    virtual void tcpServiceConnectionClosed(TCPService::ConnectionId connectionId) override {
        ++closed;
    }

    size_t established{0}, failed{0}, received{0}, closed{0};
};

TEST(TCPService, send) {
    struct Delegate : CountingDelegate {
        virtual void tcpServiceConnectionReceivedData(TCPService::ConnectionId connectionId, const void* data, size_t length) override {
            EXPECT_EQ(length, 5);
            EXPECT_EQ(memcmp(data, "hello", length), 0);

            CountingDelegate::tcpServiceConnectionReceivedData(connectionId, data, length);
        }
    } delegate;
    TCPService service{&delegate};

    EXPECT_TRUE(service.bind("127.0.0.1", 9876));
    service.start();

    auto connection = service.connect("127.0.0.1", 9876);
    EXPECT_GT(connection, 0);

    service.send(connection, "hello");

    std::this_thread::sleep_for(300ms);

    service.stop();
    service.wait();

    EXPECT_EQ(delegate.established, 2);
    EXPECT_EQ(delegate.failed, 0);
    EXPECT_EQ(delegate.received, 1);
    EXPECT_EQ(delegate.closed, 2);
}

TEST(TCPService, failedConnection) {
    CountingDelegate delegate;
    TCPService service{&delegate};

    EXPECT_TRUE(service.bind("127.0.0.1", 9876));
    service.start();

    auto connection = service.connect("127.0.0.1", 6789);
    EXPECT_GT(connection, 0);

    std::this_thread::sleep_for(300ms);

    service.stop();
    service.wait();

    EXPECT_EQ(delegate.established, 0);
    EXPECT_EQ(delegate.failed, 1);
    EXPECT_EQ(delegate.received, 0);
    EXPECT_EQ(delegate.closed, 0);
}
