#pragma once

#include "scraps/config.h"

#include "scraps/RunLoop.h"
#include "scraps/UDPSocket.h"

namespace scraps {

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

} // namespace scraps
