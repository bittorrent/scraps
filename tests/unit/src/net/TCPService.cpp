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
#include "../gtest.h"

#include <scraps/net/TCPService.h>

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
