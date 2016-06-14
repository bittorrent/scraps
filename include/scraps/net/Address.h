#pragma once

#include "scraps/config.h"

SCRAPS_IGNORE_WARNINGS_PUSH
#include <boost/asio/ip/udp.hpp>
#include <boost/functional/hash.hpp>
SCRAPS_IGNORE_WARNINGS_POP

namespace scraps {
namespace net {

class Address : public boost::asio::ip::address {
public:
    enum class Protocol {
        kIPv4,
        kIPv6,
    };

    template <typename... Args>
    Address(Args&&... args) : boost::asio::ip::address(std::forward<Args>(args)...) {}

    Protocol protocol() const { return is_v4() ? Protocol::kIPv4 : Protocol::kIPv6; }
};

}} // namespace scraps::net

namespace std {

template <>
struct hash<scraps::net::Address> {
    size_t operator()(const scraps::net::Address& address) const {
        auto data = (unsigned char*)&address;
        return boost::hash_range(data, data + sizeof(address));
    }
};

} // namespace std
