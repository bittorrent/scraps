#pragma once

#include "scraps/config.h"
#include "scraps/utility.h"

SCRAPS_IGNORE_WARNINGS_PUSH
#include <asio/ip/udp.hpp>
SCRAPS_IGNORE_WARNINGS_POP

namespace scraps {
namespace net {

class Endpoint : public asio::ip::udp::endpoint {
public:
    template <typename... Args>
    Endpoint(Args&&... args) : asio::ip::udp::endpoint(std::forward<Args>(args)...) {}

    /**
    * Used to get a sockaddr struct for the given address / port.
    */
    socklen_t getSockAddr(sockaddr_storage* storage) const;
};

}} // namespace scraps::net

namespace std {

template <>
struct hash<scraps::net::Endpoint> {
    size_t operator()(const scraps::net::Endpoint& endpoint) const {
        auto data = (unsigned char*)endpoint.data();
        return scraps::HashRange(data, data + endpoint.size());
    }
};

} // namespace std
