#pragma once

#include "scraps/config.h"

SCRAPS_IGNORE_WARNINGS_PUSH
#include <boost/asio/ip/udp.hpp>
#include <boost/functional/hash.hpp>
SCRAPS_IGNORE_WARNINGS_POP

namespace scraps {
namespace net {

class Endpoint : public boost::asio::ip::udp::endpoint {
public:
    template <typename... Args>
    Endpoint(Args&&... args) : boost::asio::ip::udp::endpoint(std::forward<Args>(args)...) {}

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
        return boost::hash_range(data, data + endpoint.size());
    }
};

} // namespace std
