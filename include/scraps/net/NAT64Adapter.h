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
#include <scraps/net/UDPSender.h>
#include <scraps/net/UDPReceiver.h>

#include <stdts/optional.h>

namespace scraps {
namespace net {

/**
* The NAT64Adapter translates IPv4 addresses to and from IPv6 address as they're sent and received.
*/
class NAT64Adapter : public UDPSender, public UDPReceiver {
public:
    using Prefix = std::array<uint8_t, 12>;

    /**
    * Constructs a new NAT64 adapter. You can use this to adopt and wrap an IPv6 UDP sender.
    */
    NAT64Adapter(Prefix prefix, std::shared_ptr<UDPSender> sender, std::weak_ptr<UDPReceiver> receiver = {})
        : _prefix(std::move(prefix)), _sender{sender}, _receiver{receiver} {}

    void setReceiver(std::weak_ptr<UDPReceiver> receiver);

    virtual bool send(const Endpoint& destination, const void* data, size_t length) override;

    virtual void receiveUDP(const net::Endpoint& sender, const void* data, size_t length) override;

    /**
    * Gets the appropriate NAT64 prefix via DNS64.
    */
    static stdts::optional<Prefix> QueryPrefix();

    static Address IPv4ToIPv6(const Prefix& prefix, const Address& address);
    static std::tuple<Prefix, Address> IPv6ToIPv4(const Address& address);

private:
    mutable std::mutex _mutex;

    Prefix _prefix;
    std::shared_ptr<UDPSender> _sender;
    std::weak_ptr<UDPReceiver> _receiver;
};

}} // namespace scraps::net
