#include "gtest/gtest.h"

#include "scraps/network.h"
#include "scraps/UDPService.h"

using namespace scraps;

struct LambdaUDPReceiver : UDPReceiver {
    LambdaUDPReceiver(std::function<void(const UDPEndpoint& sender, const void* data, size_t length)> function) : function(function) {}
    virtual void receiveUDP(const UDPEndpoint& sender, const void* data, size_t length) override {
        function(sender, data, length);
    }
    std::function<void(const UDPEndpoint& sender, const void* data, size_t length)> function;
};

static void BasicTest(UDPSocket::Protocol protocol) {
    UDPService alice, bob, carol;

    auto loopback = protocol == UDPSocket::Protocol::kIPv4 ? "127.0.0.1" : "::1";

    uint16_t alicePort = 10040;
    UDPEndpoint aliceEndpoint(Address::from_string(loopback), alicePort);

    uint16_t bobPort = 10041;
    UDPEndpoint bobEndpoint(Address::from_string(loopback), bobPort);

    uint16_t carolPort = 10042;
    UDPEndpoint carolEndpoint(Address::from_string(loopback), carolPort);

    int conversationStep = 0;
    int conversationEndStep = 5;

    std::shared_ptr<UDPSocket> aliceSocket, bobSocket, carolSocket;
    std::shared_ptr<LambdaUDPReceiver> aliceReceiver, bobReceiver, carolReceiver;

    // open alice's port
    aliceSocket = alice.openSocket(protocol, alicePort, aliceReceiver = std::make_shared<LambdaUDPReceiver>([&](const UDPEndpoint& sender, const void* data, size_t len) {
        switch (conversationStep) {
            case 1:
                ASSERT_EQ(bobEndpoint, sender);
                ASSERT_EQ(9, len);
                ASSERT_EQ(0, memcmp(data, "xhi alice", len));
                ++conversationStep;
                aliceSocket->send(bobEndpoint, "xis carol around?", 17);
                break;
            case 3:
                ASSERT_EQ(carolEndpoint, sender);
                ASSERT_EQ(27, len);
                ASSERT_EQ(0, memcmp(data, "xhey alice, i'm right here!", len));
                ++conversationStep;
                aliceSocket->send(carolEndpoint, "yoh there you are!", 18);
                break;
            default:
                conversationStep = 1000;
        }
    }));

    // open bob's port
    bobSocket = bob.openSocket(protocol, bobPort, bobReceiver = std::make_shared<LambdaUDPReceiver>([&](const UDPEndpoint& sender, const void* data, size_t len) {
        switch (conversationStep) {
            case 0:
                ASSERT_EQ(aliceEndpoint, sender);
                ASSERT_EQ(10, len);
                ASSERT_EQ(0, memcmp(data, "xhello bob", len));
                ++conversationStep;
                bobSocket->send(aliceEndpoint, "xhi alice", 9);
                break;
            case 2:
                ASSERT_EQ(aliceEndpoint, sender);
                ASSERT_EQ(17, len);
                ASSERT_EQ(0, memcmp(data, "xis carol around?", len));
                ++conversationStep;
                carolSocket->send(aliceEndpoint, "xhey alice, i'm right here!", 27);
                break;
            default:
                conversationStep = 1000;
        }
    }));

    // open carol's port
    carolSocket = carol.openSocket(protocol, carolPort, carolReceiver = std::make_shared<LambdaUDPReceiver>([&](const UDPEndpoint& sender, const void* data, size_t len) {
        switch (conversationStep) {
            case 4:
                ASSERT_EQ(aliceEndpoint, sender);
                ASSERT_EQ(18, len);
                ASSERT_EQ(0, memcmp(data, "yoh there you are!", len));
                ++conversationStep;
                break;
            default:
                conversationStep = 1000;
        }
    }));

    ASSERT_TRUE(aliceSocket->send(bobEndpoint, "xhello bob", 10));

    for (int i = 0; conversationStep < conversationEndStep && i < 16; ++i) {
        std::this_thread::sleep_for(250ms);
    }

    ASSERT_EQ(conversationEndStep, conversationStep);
}

TEST(UDPService, ipv4) {
    BasicTest(UDPSocket::Protocol::kIPv4);
};

TEST(UDPService, ipv6) {
    BasicTest(UDPSocket::Protocol::kIPv6);
};

static void MulticastTest(UDPSocket::Protocol protocol) {
    UDPService alice, bob, carol;

    uint16_t multicastPort = 30001;
    Address multicastAddress(Address::from_string(protocol == UDPSocket::Protocol::kIPv4 ? "239.255.0.1" : "ff03::1"));
    UDPEndpoint multicastEndpoint(multicastAddress, multicastPort);

    std::shared_ptr<UDPSocket> aliceSocket, bobSocket, carolSocket;
    std::shared_ptr<LambdaUDPReceiver> aliceReceiver, bobReceiver, carolReceiver;

    bool received[3][2]{};

    aliceSocket = alice.openMulticastSocket(multicastAddress, multicastPort, aliceReceiver = std::make_shared<LambdaUDPReceiver>([&](const UDPEndpoint& sender, const void* data, size_t len) {
        if (len == 15 && memcmp(data, "hello everyone!", 15)) {
            received[0][0] = true;
        }
        if (len == 10 && memcmp(data, "sup ladies", 10)) {
            received[0][1] = true;
        }
    }));

    bobSocket = bob.openMulticastSocket(multicastAddress, multicastPort, bobReceiver = std::make_shared<LambdaUDPReceiver>([&](const UDPEndpoint& sender, const void* data, size_t len) {
        if (len == 15 && memcmp(data, "hello everyone!", 15)) {
            received[1][0] = true;
        }
        if (len == 10 && memcmp(data, "sup ladies", 10)) {
            received[1][1] = true;
        }
    }));

    carolSocket = carol.openMulticastSocket(multicastAddress, multicastPort, carolReceiver = std::make_shared<LambdaUDPReceiver>([&](const UDPEndpoint& sender, const void* data, size_t len) {
        if (len == 15 && memcmp(data, "hello everyone!", 15)) {
            received[2][0] = true;
        }
        if (len == 10 && memcmp(data, "sup ladies", 10)) {
            received[2][1] = true;
        }
    }));

    ASSERT_TRUE(aliceSocket->send(multicastEndpoint, "hello everyone!", 15));
    ASSERT_TRUE(bobSocket->send(multicastEndpoint, "sup ladies", 10));

    std::this_thread::sleep_for(200ms);

    for (int i = 0; i < 6; ++i) {
        ASSERT_TRUE(received[i]);
    }
}

TEST(UDPService, multicastIPv4) {
    MulticastTest(UDPSocket::Protocol::kIPv4);
};

TEST(UDPService, multicastIPv6) {
    if (DefaultIPv6Interface().is_loopback()) {
        fprintf(stderr,
            "*************************************************\n"
            "* Warning! No IPv6 connectivity. Skipping test. *\n"
            "*************************************************\n"
        );
    } else {
        MulticastTest(UDPSocket::Protocol::kIPv6);
    }
};
