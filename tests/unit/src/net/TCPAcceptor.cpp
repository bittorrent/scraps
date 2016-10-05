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
#include "scraps/net/TCPAcceptor.h"
#include "scraps/net/TCPService.h"

#include <gtest/gtest.h>

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

TEST(TCPAcceptor, inUsePorts) {
    TCPAcceptor<TestConnection> a, b, c;
    ASSERT_TRUE(a.start(Address::from_string("127.0.0.1"), 6500));
    ASSERT_FALSE(b.start(Address::from_string("127.0.0.1"), 6500));
    ASSERT_TRUE(b.start(Address::from_string("127.0.0.1"), 6501));
    ASSERT_FALSE(c.start(Address::from_string("127.0.0.1"), 6500));
    ASSERT_FALSE(c.start(Address::from_string("127.0.0.1"), 6501));
    ASSERT_TRUE(c.start(Address::from_string("127.0.0.1"), 6502));
}

