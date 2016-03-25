#pragma once

#include "scraps/config.h"

#include "scraps/UDPReceiver.h"
#include "scraps/UDPSender.h"

#include <array>
#include <mutex>
#include <thread>

namespace scraps {

/**
* Thread-safe.
*/
class UDPSocket : public UDPSender {
public:
    enum class Protocol {
        kIPv4,
        kIPv6,
    };

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
    virtual bool send(const UDPEndpoint& destination, const void* data, size_t length) override;
    
    /**
    * Attempts to receive data on the socket and dispatch it to its receiver.
    */
    void receive();

    /**
    * Explicitly closes the socket.
    */
    void close();

private:
    std::mutex _mutex;
    int _socket = -1;
    Protocol _protocol;
    std::weak_ptr<UDPReceiver> _receiver;
    std::array<unsigned char, 4096> _buffer;

    bool _bind(const char* interface, uint16_t port);
};

} // namespace scraps
