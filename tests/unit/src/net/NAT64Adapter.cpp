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
#include <scraps/net/NAT64Adapter.h>
#include <scraps/net/UDPService.h>
#include <scraps/net/UDPSocket.h>
#include <scraps/net/utility.h>

#include <gtest/gtest.h>

#include <future>

using namespace scraps;

TEST(NAT64Adapter, basics) {
    if (net::DefaultIPv6Interface().is_loopback()) {
        fprintf(stderr,
            "*************************************************\n"
            "* Warning! No IPv6 connectivity. Skipping test. *\n"
            "*************************************************\n"
        );
        return;
    }

    struct Receiver : net::UDPReceiver {
        virtual void receiveUDP(const net::Endpoint& sender, const void* data, size_t length) override {
            EXPECT_TRUE(sender.address().is_v4());
            auto begin = reinterpret_cast<const uint8_t*>(data);
            response.set_value(std::vector<uint8_t>(begin, begin + length));
        }
        std::promise<std::vector<uint8_t>> response;
    };

    net::UDPService service;

    auto receiver = std::make_shared<Receiver>();
    auto socket = service.openSocket(net::Address::Protocol::kIPv6, 10100, receiver);

    const uint8_t packet[] = {
        0xf3, 0x84, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x69, 0x70,
        0x76, 0x34, 0x6f, 0x6e, 0x6c, 0x79, 0x04, 0x61, 0x72, 0x70, 0x61, 0x00, 0x00, 0x01, 0x00,
        0x01
    };
    auto destination = net::Endpoint(net::Address::from_string("8.8.8.8"), 53);

    // sending to the ipv4 endpoint directly shouldn't work
    EXPECT_FALSE(socket->send(destination, packet, sizeof(packet)));

    auto prefix = net::NAT64Adapter::QueryPrefix();
    EXPECT_TRUE(prefix);

    auto nat64 = std::make_shared<net::NAT64Adapter>(*prefix, socket, receiver);
    socket->setReceiver(nat64);
    EXPECT_TRUE(nat64->send(destination, packet, sizeof(packet)));

    auto futureResponse = receiver->response.get_future();
    auto status = futureResponse.wait_for(std::chrono::seconds(1));

    ASSERT_EQ(std::future_status::ready, status);
    auto response = futureResponse.get();

    const uint8_t expectedHeader[] = { 0xf3, 0x84, 0x81, 0x80, 0x00, 0x01 };

    ASSERT_GT(response.size(), 0x20);
    EXPECT_TRUE(std::equal(expectedHeader, expectedHeader + sizeof(expectedHeader), response.begin()));
}

TEST(NAT64Adapter, addressConversion) {
    net::NAT64Adapter::Prefix prefix{{1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2}};

    auto ipv4 = net::Address::from_string("8.8.8.8");
    auto mapped = net::NAT64Adapter::IPv4ToIPv6(prefix, ipv4);
    EXPECT_EQ(net::Address::from_string("0102:0304:0506:0708:0900:0102:0808:0808"), mapped);
    EXPECT_EQ(std::make_tuple(prefix, ipv4), net::NAT64Adapter::IPv6ToIPv4(mapped));
}
