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

#include <scraps/config.h>

#include <scraps/net/Address.h>
#include <scraps/net/Endpoint.h>
#include <scraps/utility.h>

#include <vector>

namespace scraps::net {

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
* Resolves the given host-port pair and returns a random endpoint.
*/
stdts::optional<Endpoint> ResolveRandomEndpoint(const std::string& hostPortPair, uint16_t defaultPort = 0);

/**
* Like ResolveRandomEndpoint, but only returns IPv4 addresses.
*/
stdts::optional<Endpoint> ResolveRandomIPv4Endpoint(const std::string& hostPortPair, uint16_t defaultPort = 0);

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

} // namespace scraps::net
