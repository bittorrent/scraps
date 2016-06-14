#include "scraps/net/utility.h"

#include "scraps/chrono.h"
#include "scraps/net/Endpoint.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include <thread>

namespace scraps {
namespace net {

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
    //               172.16.0.0   ->   172.31.255.255
    return (addr == 0x7f000001 || (addr & 0xff000000) == 0x0a000000 || (addr & 0xffff0000) == 0xc0a80000 ||
            (addr >= 0xac100000 && addr <= 0xac1fffff));
}

std::vector<Address> Resolve(const std::string& host) {
    boost::asio::io_service service;
    boost::asio::ip::udp::resolver resolver(service);
    boost::asio::ip::udp::resolver::query query(host, "");

    std::vector<Address> ret;
    boost::system::error_code ec;
    for (auto it = resolver.resolve(query, ec); !ec && it != boost::asio::ip::udp::resolver::iterator(); ++it) {
        ret.emplace_back(it->endpoint().address());
    }

    return ret;
}

std::vector<Address> ResolveIPv4(const std::string& host) {
    std::vector<Address> ret;
    auto addresses = Resolve(host);
    for (auto& address : addresses) {
        if (address.protocol() == Address::Protocol::kIPv4) {
            ret.emplace_back(address);
        }
    }
    return ret;
}

Address DefaultInterface(bool ipv6) {
    boost::asio::io_service service;
    auto thread = std::thread([&] { service.run(); });
    Address localAddress;
    try {
        boost::asio::ip::udp::socket testSocket(service);
        testSocket.connect(Endpoint(Address::from_string(ipv6 ? "2001:4860:4860::8888" : "8.8.8.8"), 9));
        localAddress = testSocket.local_endpoint().address();
    } catch (...) {
        localAddress = Address::from_string(ipv6 ? "::1" : "127.0.0.1");
    }
    service.stop();
    thread.join();
    return localAddress;
}

std::string URLEncode(const char* str) {
    std::ostringstream ret;

    while (char c = *str) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.') {
            ret << c;
        } else if (c == ' ') {
            ret << '+';
        } else {
            ret << '%' << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (int)static_cast<unsigned char>(c);
        }
        ++str;
    }

    return ret.str();
}

std::string URLDecode(const char* str) {
    std::ostringstream ret;

    while (char c = *str) {
        if (c == '+') {
            ret << ' ';
        } else if (c == '%' && str[1] && str[2]) {
            char tmp[3];
            tmp[0] = *++str;
            tmp[1] = *++str;
            tmp[2] = '\0';
            ret << (unsigned char)strtoul(tmp, nullptr, 16);
        } else {
            ret << c;
        }
        ++str;
    }

    return ret.str();
}

}} // namespace scraps::net
