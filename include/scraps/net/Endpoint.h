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
#include <scraps/utility.h>

SCRAPS_IGNORE_WARNINGS_PUSH
#include <asio/ip/udp.hpp>
SCRAPS_IGNORE_WARNINGS_POP

namespace scraps::net {

class Endpoint : public asio::ip::udp::endpoint {
public:
    template <typename... Args>
    Endpoint(Args&&... args) : asio::ip::udp::endpoint(std::forward<Args>(args)...) {}

    /**
    * Used to get a sockaddr struct and length for the given address / port.
    */
    void getSockAddr(sockaddr_storage* address, socklen_t* addressLength) const;

    /**
     * Used to construct an Endpoint from a sockaddr.
     */
    static Endpoint FromSockaddr(const sockaddr* address, socklen_t addressLength);
};

} // namespace scraps::net

namespace std {

template <>
struct hash<scraps::net::Endpoint> {
    size_t operator()(const scraps::net::Endpoint& endpoint) const {
        auto data = (unsigned char*)endpoint.data();
        return scraps::HashRange(data, data + endpoint.size());
    }
};

} // namespace std
