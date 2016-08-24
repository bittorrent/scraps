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
#pragma once

#include "scraps/config.h"

#include "scraps/net/Address.h"
#include "scraps/net/UDPReceiver.h"
#include "scraps/net/UDPSender.h"

#include <array>
#include <mutex>
#include <thread>
#include <atomic>

namespace scraps {
namespace net {

/**
* Thread-safe.
*/
class UDPSocket : public UDPSender {
public:
    using Protocol = Address::Protocol;

    static constexpr size_t kEthernetMTU = 1500;
    static constexpr size_t kIPv4HeaderSize = 20;
    static constexpr size_t kIPv6HeaderSize = 40;
    static constexpr size_t kUDPHeaderSize = 8;
    static constexpr size_t kMaxIPv4UDPPayloadSize = kEthernetMTU - kIPv4HeaderSize - kUDPHeaderSize;
    static constexpr size_t kMaxIPv6UDPPayloadSize = kEthernetMTU - kIPv6HeaderSize - kUDPHeaderSize;

    /**
    * Creates a new UDP socket.
    *
    * @param receiver must outlive the socket. if null, should be given via setReceiver
    */
    UDPSocket(Protocol protocol, std::weak_ptr<UDPReceiver> receiver = std::weak_ptr<UDPReceiver>());

    ~UDPSocket() { close(); }

    /**
    * Returns the native socket.
    */
    int native() const { return _socket; }

    /**
    * Sets the receiver for the socket. The receiver should outlive the socket and
    * should not be changed once set.
    */
    void setReceiver(std::weak_ptr<UDPReceiver> receiver);

    /**
    * Binds the socket to any interface on the given port.
    */
    bool bind(uint16_t port);

    /**
    * Binds the socket to the given interface and port.
    */
    bool bind(const char* interface, uint16_t port);

    /**
    * Binds the socket, joining the given multicast group address.
    */
    bool bindMulticast(const char* groupAddress, uint16_t port);

    /**
    * Sends data on the socket.
    */
    virtual bool send(const Endpoint& destination, const void* data, size_t length) override;

    /**
    * Attempts to receive data on the socket and dispatch it to its receiver.
    */
    void receive();

    /**
    * Explicitly closes the socket.
    */
    void close();

    /**
     * Get total number of bytes sent and received.
     */
    uint64_t totalSentBytes() { return _totalSentBytes; }
    uint64_t totalReceivedBytes() { return _totalReceivedBytes; }

private:
    std::mutex _mutex;
    int _socket = -1;
    Protocol _protocol;
    std::weak_ptr<UDPReceiver> _receiver;
    std::array<unsigned char, 4096> _buffer;

    std::atomic_uint_fast64_t _totalSentBytes{0};
    std::atomic_uint_fast64_t _totalReceivedBytes{0};

    bool _bind(const char* interface, uint16_t port);
};

}} // namespace scraps::net
