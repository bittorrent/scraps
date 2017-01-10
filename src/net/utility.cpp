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
#include <scraps/net/utility.h>

#include <scraps/chrono.h>
#include <scraps/net/Endpoint.h>

#include <unistd.h>

#include <thread>

namespace scraps::net {

void ShutdownAndCloseTCPSocket(int fd) {
#if _WIN32
    shutdown(fd, SD_SEND);
#else
    shutdown(fd, SHUT_WR);
#endif

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    auto timeout = ToTimeval(1s);
    if (select(fd + 1, &rfds, nullptr, nullptr, &timeout) > 0) {
        recv(fd, nullptr, 0, 0);
    }

#if _WIN32
    closesocket(fd);
#else
    close(fd);
#endif
}

bool IsLocal(const Address& address) {
    if (!address.is_v4()) {
        // TODO: support v6?
        return false;
    }

    auto addr = address.to_v4().to_ulong();

    //               127.0.0.1                             10.x.x.x                          192.168.x.x
    return (addr == 0x7f000001 || (addr & 0xff000000) == 0x0a000000 || (addr & 0xffff0000) == 0xc0a80000 ||
    //               172.16.0.0   ->   172.31.255.255
            (addr >= 0xac100000 && addr <= 0xac1fffff));
}

std::vector<Address> Resolve(const std::string& host) {
    asio::io_service service;
    asio::ip::udp::resolver resolver(service);
    asio::ip::udp::resolver::query query(host, "");

    std::vector<Address> ret;
    asio::error_code ec;
    for (auto it = resolver.resolve(query, ec); !ec && it != asio::ip::udp::resolver::iterator(); ++it) {
        ret.emplace_back(it->endpoint().address());
    }

    return ret;
}

std::vector<Address> Resolve(const std::string& host, Address::Protocol protocol) {
    std::vector<Address> ret;
    auto addresses = Resolve(host);
    for (auto& address : addresses) {
        if (address.protocol() == protocol) {
            ret.emplace_back(address);
        }
    }
    return ret;
}

stdts::optional<Endpoint> ResolveRandomEndpoint(const std::string& hostPortPair, uint16_t defaultPort) {
    auto parsed = ParseAddressAndPort(hostPortPair, defaultPort);
    if (!std::get<1>(parsed)) { return {}; }
    auto addresses = Resolve(std::get<0>(parsed));
    if (addresses.empty()) { return {}; }
    std::default_random_engine rng{static_cast<std::default_random_engine::result_type>(std::chrono::system_clock::now().time_since_epoch().count())};
    return Endpoint{addresses[UniformDistribution(0u, addresses.size() - 1, rng)], std::get<1>(parsed)};
}

stdts::optional<Endpoint> ResolveRandomIPv4Endpoint(const std::string& hostPortPair, uint16_t defaultPort) {
    auto parsed = ParseAddressAndPort(hostPortPair, defaultPort);
    if (!std::get<1>(parsed)) { return {}; }
    auto addresses = ResolveIPv4(std::get<0>(parsed));
    if (addresses.empty()) { return {}; }
    std::default_random_engine rng{static_cast<std::default_random_engine::result_type>(std::chrono::system_clock::now().time_since_epoch().count())};
    return Endpoint{addresses[UniformDistribution(0u, addresses.size() - 1, rng)], std::get<1>(parsed)};
}

Address DefaultInterface(bool ipv6) {
    asio::io_service service;
    auto thread = std::thread([&] { service.run(); });
    Address localAddress;
    try {
        asio::ip::udp::socket testSocket(service);
        testSocket.connect(Endpoint(Address::from_string(ipv6 ? "2001:4860:4860::8888" : "8.8.8.8"), 9));
        localAddress = testSocket.local_endpoint().address();
    } catch (...) {
        localAddress = Address::from_string(ipv6 ? "::1" : "127.0.0.1");
    }
    service.stop();
    thread.join();
    return localAddress;
}

} // namespace scraps::net
