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
