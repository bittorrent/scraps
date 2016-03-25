#pragma once

#include "scraps/config.h"

#include "scraps/types.h"

namespace scraps {

/**
* Returns the code for the most recent socket error
*
* @return the code for the most recent socket error
*/
inline int SocketError() {
#if WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

/**
* Sets the given file descriptor to blocking or non-blocking.
*
* @param fd the file descriptor
* @param blocking true if the file descriptor should be set to blocking. false if it should be set to non-blocking
* @return true on success
*/
bool SetBlocking(int fd, bool blocking = true);

/**
* Gracefully shuts down and closes a socket.
*
* @param fd the socket
*/
void ShutdownAndCloseTCPSocket(int fd);

/**
* Returns true if the given address appears to be a lan address. This function currently
* only supports IPv4 addresses.
*
* @return true if the given address appears to be a lan address
*/
bool IsLocal(const Address& address);

/**
* Performs a blocking resolution of the given ipv4 address.
*
* @param address the address to resolve, optionally with a port specified in the format host:port
* @param ec the resulting error code
* @return the resolved endpoint
*/
UDPEndpoint ResolveIPv4(const char* address, boost::system::error_code& ec);

/**
* Returns the address for the default Internet-facing interface. If there are none available,
* the loopback interface is returned. For readability, using DefaultIPv4Interface or
* DefaultIPv6Interface is generally preferred.
*
* @param ipv6 if true, an ipv6 interface is returned. otherwise, an ipv4 interface is returned.
*/
Address DefaultInterface(bool ipv6 = false);

inline Address DefaultIPv4Interface() { return DefaultInterface(false); }
inline Address DefaultIPv6Interface() { return DefaultInterface(true); }

/**
* Used to get a sockaddr struct for the given address / port.
*/
void GetSockAddr(Address address, uint16_t port, sockaddr_storage* storage, socklen_t* length);

} // namespace scraps
