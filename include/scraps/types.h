#pragma once

#include "scraps/config.h"

SCRAPS_IGNORE_WARNINGS_PUSH
#include <boost/asio/ip/udp.hpp>
#include <boost/functional/hash.hpp>
SCRAPS_IGNORE_WARNINGS_POP

namespace scraps {

typedef boost::asio::ip::address Address;
typedef boost::asio::ip::udp::endpoint UDPEndpoint;

} // namespace scraps

namespace std {

template <>
struct hash<scraps::Address> {
    size_t operator()(const scraps::Address& address) const {
        auto data = (unsigned char*)&address;
        return boost::hash_range(data, data + sizeof(address));
    }
};
template <>
struct hash<scraps::UDPEndpoint> {
    size_t operator()(const scraps::UDPEndpoint& endpoint) const {
        auto data = (unsigned char*)endpoint.data();
        return boost::hash_range(data, data + endpoint.size());
    }
};

} // namespace std
