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
#include "scraps/logging.h"
#include "scraps/net/HTTPConnection.h"
#include "scraps/net/HTTPRequest.h"
#include "scraps/net/TCPAcceptor.h"

#include <gtest/gtest.h>

using namespace scraps;
using namespace scraps::net;

class TestHTTPConnection : public HTTPConnection {
public:
    TestHTTPConnection(int socket, const std::chrono::seconds& timeout) : HTTPConnection(socket, timeout) {
        lastResult = kResultUnknown;
    }

    ~TestHTTPConnection() { lastResult = result(); }

    virtual void handleRequest(const Request& request) override {
        std::string response = "foobar";

        sendResponse("HTTP/1.1 200 OK", response.data(), response.size());
    }

    static ResultType lastResult;
};

HTTPConnection::ResultType TestHTTPConnection::lastResult = HTTPConnection::kResultUnknown;

using ServerType = TCPAcceptor<TestHTTPConnection, std::chrono::seconds>;

TEST(HTTPConnection, normalOperation) {
    std::chrono::seconds timeout = 1s;
    uint16_t httpPort = 3456;
    auto httpServer = std::make_shared<ServerType>(timeout);
    ASSERT_TRUE(httpServer->start(Address::from_string("127.0.0.1"), httpPort));

    {
        HTTPRequest request(Formatf("http://127.0.0.1:%d/test", httpPort));
        request.wait();

        ASSERT_TRUE(request.isComplete());
        ASSERT_FALSE(request.error());
        ASSERT_EQ(200, request.responseStatus());

        ASSERT_EQ("foobar", request.responseBody());
    }

    httpServer->stop();

    ASSERT_EQ(HTTPConnection::kResultSuccess, TestHTTPConnection::lastResult);
}

TEST(HTTPConnection, timeout) {
    std::chrono::seconds timeout = 1s;
    std::string host = "127.0.0.1";
    uint16_t httpPort = 3456;
    auto httpServer = std::make_shared<ServerType>(timeout);
    ASSERT_TRUE(httpServer->start(Address::from_string(host), httpPort));

    {
        asio::io_service service;

        asio::ip::tcp::endpoint endpoint(Address::from_string(host), httpPort);

        asio::ip::tcp::socket socket(service);
        socket.open(asio::ip::tcp::v4());
        socket.connect(endpoint);

        // don't send anything to simulate a timeout

        std::this_thread::sleep_for(2s);
    }

    httpServer->stop();

    ASSERT_EQ(HTTPConnection::kResultTimeout, TestHTTPConnection::lastResult);
};

TEST(HTTPConnection, abort) {
    std::chrono::seconds timeout = 1s;
    std::string host = "127.0.0.1";
    uint16_t httpPort = 3456;
    auto httpServer = std::make_shared<ServerType>(timeout);
    ASSERT_TRUE(httpServer->start(Address::from_string(host), httpPort));

    {
        asio::io_service service;

        asio::ip::tcp::endpoint endpoint(Address::from_string(host), httpPort);

        asio::ip::tcp::socket socket(service);
        socket.open(asio::ip::tcp::v4());
        socket.connect(endpoint);

        std::this_thread::sleep_for(200ms);

        // close before timeout occurs
        socket.close();
    }

    httpServer->stop();

    ASSERT_EQ(HTTPConnection::kResultSocketClosedByPeer, TestHTTPConnection::lastResult);
}
