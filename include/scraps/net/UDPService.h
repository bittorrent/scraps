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

#include "scraps/RunLoop.h"
#include "scraps/net/UDPSocket.h"

namespace scraps {
namespace net {

/**
* Service for sending / receiving data via UDP. The service owns a thread that it will use to invoke socket methods.
*
* Thread-safe.
*/
class UDPService {
public:
    UDPService();
    ~UDPService();

    /**
    * Opens a socket bound to the given port.
    *
    * @param interface the interface to open the port on (e.g. "127.0.0.1"). pass nullptr to open on any interface
    */
    std::shared_ptr<UDPSocket> openSocket(UDPSocket::Protocol protocol, uint16_t port, std::weak_ptr<UDPReceiver> receiver = std::weak_ptr<UDPReceiver>(), const char* interface = nullptr);

    /**
    * Opens a socket in the given multicast groupd and returns a UDPSocket object.
    *
    * @param groupAddress the multicast address for the group
    * @param port the port number to open
    */
    std::shared_ptr<UDPSocket> openMulticastSocket(const Address& groupAddress, uint16_t port, std::weak_ptr<UDPReceiver> receiver = std::weak_ptr<UDPReceiver>());

    std::shared_ptr<UDPSocket> openMulticastSocket(const char* groupAddress, uint16_t port, std::weak_ptr<UDPReceiver> receiver = std::weak_ptr<UDPReceiver>()) {
        return openMulticastSocket(Address::from_string(groupAddress), port, receiver);
    }

    /**
    * Returns the service's run loop.
    */
    RunLoop& runLoop() { return _runLoop; }

    void kill();

private:
    std::mutex _mutex;
    RunLoop _runLoop;
    std::thread _thread;
    std::unordered_map<int, std::weak_ptr<UDPSocket>> _sockets;

    void _purgeDeadSockets();
};

}} // namespace scraps::net
