#pragma once

#include "scraps/config.h"

#include "scraps/net/Address.h"

#include <vector>

namespace scraps {
namespace net {

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
* Performs a blocking resolution of the given host.
*
* @param address the host to resolve
* @return the resolved addresses
*/
std::vector<Address> Resolve(const std::string& host);

/**
* Like Resolve, but only returns IPv4 addresses.
*/
std::vector<Address> ResolveIPv4(const std::string& host);

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
* Returns a string in which all non-alphanumeric characters except dashes, underscores,
* spaces, and periods are replaced with a percent sign followed by their hexadecimal
* value. Spaces are replaced with plus signs.
*
* @return the url-encoded string
*/
std::string URLEncode(const char* str);

inline std::string URLEncode(const std::string& str) { return URLEncode(str.c_str()); }

/**
* Returns a string in which the effects of URLEncode have been reversed.
*
* @return the url-decoded string
*/
std::string URLDecode(const char* str);

inline std::string URLDecode(const std::string& str) { return URLDecode(str.c_str()); }

}} // namespace scraps::net
