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
#include <scraps/net/Endpoint.h>

#include <cassert>

namespace scraps::net {

void Endpoint::getSockAddr(sockaddr_storage* storage, socklen_t* storageLength) const {
    memset(storage, 0, sizeof(sockaddr_storage));

    if (address().is_v4()) {
        auto addr        = reinterpret_cast<sockaddr_in*>(storage);
        addr->sin_family = AF_INET;
        auto bytes = address().to_v4().to_bytes();
        static_assert(sizeof(addr->sin_addr) == sizeof(bytes), "size mismatch");
        memcpy(&addr->sin_addr, &bytes, sizeof(bytes));
        addr->sin_port = htons(port());
        *storageLength = sizeof(*addr);
    } else if (address().is_v6()) {
        auto ipv6         = address().to_v6();
        auto addr         = reinterpret_cast<sockaddr_in6*>(storage);
        addr->sin6_family = AF_INET6;
        auto bytes = ipv6.to_bytes();
        static_assert(sizeof(addr->sin6_addr) == sizeof(bytes), "size mismatch");
        memcpy(&addr->sin6_addr, &bytes, sizeof(bytes));
        addr->sin6_port     = htons(port());
        addr->sin6_scope_id = htonl(ipv6.scope_id());
        *storageLength = sizeof(*addr);
    } else {
        assert(false);
    }
}

Endpoint Endpoint::FromSockaddr(const sockaddr* address, socklen_t addressLength) {
    if (address->sa_family == AF_INET && addressLength == sizeof(sockaddr_in)) {
        auto sa = reinterpret_cast<const sockaddr_in*>(address);
        return scraps::net::Endpoint(
            asio::ip::address_v4(
                *reinterpret_cast<const asio::ip::address_v4::bytes_type*>(&sa->sin_addr)
            ), ntohs(sa->sin_port)
        );
    } else if (address->sa_family == AF_INET6 && addressLength == sizeof(sockaddr_in6)) {
        auto sa = reinterpret_cast<const sockaddr_in6*>(address);
        return scraps::net::Endpoint(
            asio::ip::address_v6(
                *reinterpret_cast<const asio::ip::address_v6::bytes_type*>(&sa->sin6_addr),
                ntohl(sa->sin6_scope_id)
            ), ntohs(sa->sin6_port)
        );
    }
    assert(false);
    return {};
}

} // namespace scraps::net
