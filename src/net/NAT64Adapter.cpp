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
#include <scraps/net/NAT64Adapter.h>

#include <scraps/net/utility.h>

namespace scraps {
namespace net {

void NAT64Adapter::setReceiver(std::weak_ptr<UDPReceiver> receiver) {
    std::lock_guard<std::mutex> lock(_mutex);
    _receiver = receiver;
}

bool NAT64Adapter::send(const Endpoint& destination, const void* data, size_t length) {
    auto address = destination.address().is_v4() ? IPv4ToIPv6(_prefix, destination.address()) : destination.address();
    return _sender->send(Endpoint(address, destination.port()), data, length);
}

void NAT64Adapter::receiveUDP(const net::Endpoint& sender, const void* data, size_t length) {
    std::shared_ptr<UDPReceiver> receiver;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        receiver = _receiver.lock();
    }
    if (!receiver) { return; }

    if (sender.address().is_v4()) {
        return receiver->receiveUDP(sender, data, length);
    }

    auto ipv4 = IPv6ToIPv4(sender.address());
    if (std::get<Prefix>(ipv4) != _prefix) {
        return receiver->receiveUDP(sender, data, length);
    }

    return receiver->receiveUDP(Endpoint(std::get<Address>(ipv4), sender.port()), data, length);
}

stdts::optional<NAT64Adapter::Prefix> NAT64Adapter::QueryPrefix() {
    auto resolution = ResolveIPv6("ipv4only.arpa");
    if (resolution.empty()) {
        return {};
    }
    auto bytes = resolution[0].to_v6().to_bytes();
    return *reinterpret_cast<const Prefix*>(bytes.data());
}

net::Address NAT64Adapter::IPv4ToIPv6(const Prefix& prefix, const Address& address) {
    assert(address.is_v4());
    asio::ip::address_v6::bytes_type ipv6;
    std::copy_n(prefix.begin(), prefix.size(), ipv6.begin());
    auto ipv4 = address.to_v4().to_bytes();
    std::copy(ipv4.begin(), ipv4.end(), ipv6.begin() + 12);
    return net::Address(asio::ip::address_v6(ipv6));
}

std::tuple<NAT64Adapter::Prefix, net::Address> NAT64Adapter::IPv6ToIPv4(const Address& address) {
    assert(address.is_v6());
    const auto bytes = address.to_v6().to_bytes();
    auto p = *reinterpret_cast<const Prefix*>(bytes.data());
    auto a = net::Address(asio::ip::address_v4(*reinterpret_cast<const asio::ip::address_v4::bytes_type*>(bytes.data() + 12)));
    return {p, a};
}

}} // namespace scraps::net
