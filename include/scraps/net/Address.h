#pragma once

#include "scraps/config.h"
#include "scraps/utility.h"

SCRAPS_IGNORE_WARNINGS_PUSH
#include <asio/ip/udp.hpp>
SCRAPS_IGNORE_WARNINGS_POP

namespace scraps {
namespace net {

class Address : public asio::ip::address {
public:
    enum class Protocol {
        kIPv4,
        kIPv6,
    };

    template <typename... Args>
    Address(Args&&... args) : asio::ip::address(std::forward<Args>(args)...) {}

    Protocol protocol() const { return is_v4() ? Protocol::kIPv4 : Protocol::kIPv6; }
};

}} // namespace scraps::net

namespace std {

template <>
struct hash<scraps::net::Address> {
    size_t operator()(const scraps::net::Address& address) const {
        auto data = (unsigned char*)&address;
        return scraps::HashRange(data, data + sizeof(address));
    }
};

} // namespace std
