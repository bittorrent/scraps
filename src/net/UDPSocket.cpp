#include "scraps/net/UDPSocket.h"

#include "scraps/logging.h"
#include "scraps/utility.h"
#include "scraps/net/utility.h"

#include <gsl.h>

namespace scraps {
namespace net {

constexpr size_t UDPSocket::kEthernetMTU;
constexpr size_t UDPSocket::kIPv4HeaderSize;
constexpr size_t UDPSocket::kIPv6HeaderSize;
constexpr size_t UDPSocket::kUDPHeaderSize;
constexpr size_t UDPSocket::kMaxIPv4UDPPayloadSize;
constexpr size_t UDPSocket::kMaxIPv6UDPPayloadSize;

UDPSocket::UDPSocket(Protocol protocol, std::weak_ptr<UDPReceiver> receiver)
    : _socket{::socket(protocol == Protocol::kIPv4 ? AF_INET : AF_INET6, SOCK_DGRAM, 0)}
    , _protocol{protocol}
    , _receiver{receiver}
{
    if (!SetBlocking(_socket, false)) {
        SCRAPS_LOGF_ERROR("error making socket non-blocking");
    }

    constexpr int bufferSize = 128 * 1024;
    setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));
    setsockopt(_socket, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
}

void UDPSocket::setReceiver(std::weak_ptr<UDPReceiver> receiver) {
    std::lock_guard<std::mutex> lock(_mutex);
    _receiver = receiver;
}

bool UDPSocket::bind(uint16_t port) {
    return bind(nullptr, port);
}

bool UDPSocket::bind(const char* interface, uint16_t port) {
    std::lock_guard<std::mutex> lock(_mutex);
    return _bind(interface, port);
}

bool UDPSocket::bindMulticast(const char* groupAddress, uint16_t port) {
    std::lock_guard<std::mutex> lock(_mutex);

    constexpr int one = 1;
    setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
#if SO_REUSEPORT
    // this is only necessary on certain platforms.
    // on platforms where it's not necessary, it doesn't hurt. on platforms where it doesn't exist, hope it's not necessary
    setsockopt(_socket, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one));
#endif

    auto interfaceString = DefaultInterface(_protocol == Protocol::kIPv6).to_string();

    if (_protocol == Protocol::kIPv4) {
        in_addr inAddr;
        inet_pton(AF_INET, interfaceString.c_str(), &inAddr);
        setsockopt(_socket, IPPROTO_IP, IP_MULTICAST_IF, &inAddr, sizeof(inAddr));

        constexpr int two = 2;
        setsockopt(_socket, IPPROTO_IP, IP_MULTICAST_TTL, &two, sizeof(two));
    } else {
        in6_addr inAddr;
        inet_pton(AF_INET6, interfaceString.c_str(), &inAddr);
        setsockopt(_socket, IPPROTO_IP, IPV6_MULTICAST_IF, &inAddr, sizeof(inAddr));
    }

#if _WIN32
        if (!_bind(interfaceString.c_str(), port)) { return false; }
#else
        if (!_bind(nullptr, port)) { return false; }
#endif

    if (_protocol == Protocol::kIPv4) {
        ip_mreqn req;
        memset(&req, 0, sizeof(req));
        inet_pton(AF_INET, groupAddress, &req.imr_multiaddr);
        inet_pton(AF_INET, interfaceString.c_str(), &req.imr_address);
        setsockopt(_socket, IPPROTO_IP, IPV6_JOIN_GROUP, &req, sizeof(req));
    } else {
        ipv6_mreq req;
        memset(&req, 0, sizeof(req));
        inet_pton(AF_INET6, groupAddress, &req.ipv6mr_multiaddr);
        setsockopt(_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &req, sizeof(req));
    }

    return true;
}

bool UDPSocket::send(const Endpoint& destination, const void* data, size_t length) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_socket < 0) { return false; }

    sockaddr_storage addr;
    socklen_t addrLength = destination.getSockAddr(&addr);
    int sent = ::sendto(_socket, data, length, 0, reinterpret_cast<sockaddr*>(&addr), addrLength);

    if (sent < 0) {
        SCRAPS_LOG_ERROR("udp socket send error (errno = {})", static_cast<int>(errno));
    } else if (sent != length) {
        SCRAPS_LOG_WARNING("udp socket sent != length ({} != {})", sent, length);
    }

    if ((destination.address().is_v4() && sent > kMaxIPv4UDPPayloadSize) ||
        (destination.address().is_v6() && sent > kMaxIPv6UDPPayloadSize)) {
        SCRAPS_LOG_WARNING("udp socket sent {} bytes, which is over the max udp payload size", sent);
    }

    _totalSentBytes += sent;

    return sent == length;
}

void UDPSocket::receive() {
    while (true) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_socket < 0) { return; }

        sockaddr_storage senderStorage;
        socklen_t senderStorageLength = sizeof(senderStorage);

        auto bytes = ::recvfrom(_socket, &_buffer, sizeof(_buffer), 0, reinterpret_cast<sockaddr*>(&senderStorage), &senderStorageLength);

        if (bytes < 0 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
            break;
        }

        if (bytes <= 0) {
            if (bytes < 0) {
                SCRAPS_LOG_ERROR("udp socket error (errno = {})", static_cast<int>(errno));
            }
            return;
        }

        _totalReceivedBytes += bytes;

        Endpoint sender;

        if (senderStorage.ss_family == AF_INET && senderStorageLength == sizeof(sockaddr_in)) {
            auto sa = reinterpret_cast<sockaddr_in*>(&senderStorage);
            sender = Endpoint(boost::asio::ip::address_v4(*reinterpret_cast<boost::asio::ip::address_v4::bytes_type*>(&sa->sin_addr)), ntohs(sa->sin_port));
        } else if (senderStorage.ss_family == AF_INET6 && senderStorageLength == sizeof(sockaddr_in6)) {
            auto sa = reinterpret_cast<sockaddr_in6*>(&senderStorage);
            sender = Endpoint(boost::asio::ip::address_v6(*reinterpret_cast<boost::asio::ip::address_v6::bytes_type*>(&sa->sin6_addr), ntohl(sa->sin6_scope_id)), ntohs(sa->sin6_port));
        } else {
            SCRAPS_ASSERT(false);
        }

        if (auto receiver = _receiver.lock()) {
            lock.unlock();
            receiver->receiveUDP(sender, &_buffer, static_cast<size_t>(bytes));
        }
    }
}

void UDPSocket::close() {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_socket < 0) { return; }

#if _WIN32
    ::shutdown(_socket, SD_BOTH);
    ::closesocket(_socket);
#else
    ::shutdown(_socket, SHUT_RDWR);
    ::close(_socket);
#endif

    _socket = -1;
    _receiver.reset();
    _totalReceivedBytes = 0;
    _totalSentBytes = 0;
}

bool UDPSocket::_bind(const char* interface, uint16_t port) {
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    addrinfo* result = nullptr;

    hints.ai_family = _protocol == Protocol::kIPv4 ? AF_INET : AF_INET6;

    if (!interface) {
        hints.ai_flags |= AI_PASSIVE;
    }

    if (getaddrinfo(interface, Formatf("%hu", port).c_str(), &hints, &result)) {
        SCRAPS_LOG_ERROR("error interpreting socket interface");
        return false;
    }

    auto _ = gsl::finally([&] {
        freeaddrinfo(result);
    });

    if (::bind(_socket, result->ai_addr, result->ai_addrlen)) {
        SCRAPS_LOG_ERROR("error binding udp socket (errno = {})", static_cast<int>(errno));
        return false;
    }

    return true;
}

}} // namespace scraps::net
