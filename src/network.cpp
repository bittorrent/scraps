#include "scraps/network.h"

#include "scraps/utility.h"

#include <fcntl.h>
#include <unistd.h>

#include <thread>

namespace scraps {

bool SetBlocking(int fd, bool blocking) {
#ifdef _WIN32
    unsigned long arg = blocking ? 1 : 0;
    return !ioctlsocket(fd, FIONBIO, &arg);
#else
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return false;
    }
    return !fcntl(fd, F_SETFL, blocking ? flags & ~O_NONBLOCK : flags | O_NONBLOCK);
#endif
}

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

UDPEndpoint ResolveIPv4(const char* address, boost::system::error_code& ec) {
    std::string str(address);

    auto colon       = str.find(':');
    std::string host = str.substr(0, colon);
    std::string port = colon == std::string::npos ? "" : str.substr(colon + 1);

    boost::asio::io_service service;
    boost::asio::ip::udp::resolver resolver(service);
    boost::asio::ip::udp::resolver::query query(host, port);

    bool success = false;
    UDPEndpoint endpoint;
    for (auto it = resolver.resolve(query, ec); !ec && it != boost::asio::ip::udp::resolver::iterator(); ++it) {
        if (it->endpoint().address().is_v4()) {
            endpoint = it->endpoint();
            success  = true;
            break;
        }
    }

    if (!ec && !success) {
        ec = boost::system::error_code(boost::system::errc::errc_t::invalid_argument, boost::system::system_category());
    }

    return endpoint;
}

Address DefaultInterface(bool ipv6) {
    boost::asio::io_service service;
    auto thread = std::thread([&] { service.run(); });
    Address localAddress;
    try {
        boost::asio::ip::udp::socket testSocket(service);
        testSocket.connect(UDPEndpoint(Address::from_string(ipv6 ? "2001:4860:4860::8888" : "8.8.8.8"), 9));
        localAddress = testSocket.local_endpoint().address();
    } catch (...) {
        localAddress = Address::from_string(ipv6 ? "::1" : "127.0.0.1");
    }
    service.stop();
    thread.join();
    return localAddress;
}

void GetSockAddr(Address address, uint16_t port, sockaddr_storage* storage, socklen_t* length) {
    memset(storage, 0, sizeof(*storage));

    if (address.is_v4()) {
        auto addr        = reinterpret_cast<sockaddr_in*>(storage);
        addr->sin_family = AF_INET;
        auto bytes = address.to_v4().to_bytes();
        static_assert(sizeof(addr->sin_addr) == sizeof(bytes), "size mismatch");
        memcpy(&addr->sin_addr, &bytes, sizeof(bytes));
        addr->sin_port = htons(port);
        *length = sizeof(*addr);
    } else if (address.is_v6()) {
        auto ipv6         = address.to_v6();
        auto addr         = reinterpret_cast<sockaddr_in6*>(storage);
        addr->sin6_family = AF_INET6;
        auto bytes = ipv6.to_bytes();
        static_assert(sizeof(addr->sin6_addr) == sizeof(bytes), "size mismatch");
        memcpy(&addr->sin6_addr, &bytes, sizeof(bytes));
        addr->sin6_port     = htons(port);
        addr->sin6_scope_id = htonl(ipv6.scope_id());
        *length             = sizeof(*addr);
    } else {
        SCRAPS_ASSERT(false);
        *length = 0;
    }
}

} // namespace scraps
