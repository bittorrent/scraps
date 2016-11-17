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

class Address : public asio::ip::address {
public:
    enum class Protocol {
        kIPv4,
        kIPv6,
    };

    template <typename... Args>
    Address(Args&&... args) : asio::ip::address(std::forward<Args>(args)...) {}

    Protocol protocol() const { return is_v4() ? Protocol::kIPv4 : Protocol::kIPv6; }
};

} // namespace scraps::net

namespace std {

template <>
struct hash<scraps::net::Address> {
    size_t operator()(const scraps::net::Address& address) const {
        auto data = (unsigned char*)&address;
        return scraps::HashRange(data, data + sizeof(address));
    }
};

} // namespace std
